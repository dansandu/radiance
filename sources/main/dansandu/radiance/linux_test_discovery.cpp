#if defined(__linux__)
#include "dansandu/radiance/linux_test_discovery.hpp"
#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/macro.hpp"
#include "dansandu/radiance/test_class_descriptor.hpp"
#include "dansandu/radiance/utility.hpp"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <regex>
#include <set>
#include <string>
#include <vector>

#include <elf.h>
#include <link.h>

using dansandu::journey::utility::tryDemangle;
using dansandu::radiance::macro::testMethodPattern;
using dansandu::radiance::test_class_descriptor::TestMethodDescriptor;
using dansandu::radiance::utility::isSameFile;

namespace dansandu::radiance::linux_test_discovery
{

namespace
{

// Computes the number of symbols in the dynamic symbol table from the DT_GNU_HASH table, which doesn't store it
// explicitly. The table layout is: bucket count, symbol offset, bloom filter size, bloom filter shift, the bloom filter
// words, the buckets and finally the hash chain.
std::size_t getSymbolCountFromGnuHashTable(const std::uint32_t* const gnuHashTable)
{
    const auto bucketCount = gnuHashTable[0];
    const auto symbolOffset = gnuHashTable[1];
    const auto bloomFilterSize = gnuHashTable[2];

    // The bloom filter words have the size of an address (32 bits for ELF32 and 64 bits for ELF64).
    const auto bloomFilter = reinterpret_cast<const ElfW(Addr)*>(gnuHashTable + 4);
    const auto buckets = reinterpret_cast<const std::uint32_t*>(bloomFilter + bloomFilterSize);
    const auto chain = buckets + bucketCount;

    // Each bucket holds the index of the first symbol in its chain, so the highest index belongs to the last chain.
    auto highestBucketSymbolIndex = std::uint32_t{0};

    for (auto index = std::uint32_t{0}; index < bucketCount; ++index)
    {
        highestBucketSymbolIndex = std::max(highestBucketSymbolIndex, buckets[index]);
    }

    // All buckets are empty, so the table contains only the symbols that aren't hashed.
    if (highestBucketSymbolIndex < symbolOffset)
    {
        return symbolOffset;
    }

    // Walk the last chain until the entry with the stop bit (the lowest bit) is found.
    auto lastSymbolIndex = highestBucketSymbolIndex;

    while ((chain[lastSymbolIndex - symbolOffset] & 1u) == 0u)
    {
        ++lastSymbolIndex;
    }

    return lastSymbolIndex + 1;
}

// The dynamic section stores the addresses of the symbol, string and hash tables as link-time virtual addresses. The
// glibc loader relocates them in place to absolute addresses on most architectures, but musl and glibc on
// architectures with a read-only dynamic section (e.g. MIPS and RISC-V) leave them unchanged. Link-time addresses of
// a shared library are smaller than its load address, so they can be told apart from relocated addresses.
template<typename T>
const T* resolveDynamicAddress(const struct dl_phdr_info* const info, const ElfW(Addr) address)
{
    const auto absoluteAddress = address < info->dlpi_addr ? info->dlpi_addr + address : address;

    return reinterpret_cast<const T*>(absoluteAddress);
}

struct LibrarySymbol
{
    std::string symbolName;
    std::string testMethodName;
    int lineNumber;
    void* address;
};

struct SearchContext
{
    std::string libraryName;
    std::set<std::string> classNames;
    std::string methodPattern;
    bool libraryFound;
    std::map<std::string, std::vector<LibrarySymbol>> librarySymbolsByClassName;
};

int searchLibrary(struct dl_phdr_info* const libraryInformation, const std::size_t, void* const data)
{
    const auto context = static_cast<SearchContext*>(data);

    // The name is empty for the main executable.
    if (!libraryInformation->dlpi_name || !isSameFile(libraryInformation->dlpi_name, context->libraryName))
    {
        // Continue with the next loaded library.
        return 0;
    }

    context->libraryFound = true;

    // Locate the dynamic section in the program headers of the library.
    const ElfW(Dyn)* dynamicEntry = nullptr;

    for (auto index = ElfW(Half){0}; index < libraryInformation->dlpi_phnum; ++index)
    {
        const auto& programHeader = libraryInformation->dlpi_phdr[index];

        if (programHeader.p_type == PT_DYNAMIC)
        {
            dynamicEntry = reinterpret_cast<const ElfW(Dyn)*>(libraryInformation->dlpi_addr + programHeader.p_vaddr);
            break;
        }
    }

    // Returning a value other than zero stops the iteration, since the library was found.
    if (!dynamicEntry)
    {
        return 1;
    }

    const ElfW(Sym)* symbolTable = nullptr;
    const char* stringTable = nullptr;
    const std::uint32_t* gnuHashTable = nullptr;
    const std::uint32_t* systemVHashTable = nullptr;

    for (; dynamicEntry->d_tag != DT_NULL; ++dynamicEntry)
    {
        switch (dynamicEntry->d_tag)
        {
        case DT_SYMTAB:
            symbolTable = resolveDynamicAddress<ElfW(Sym)>(libraryInformation, dynamicEntry->d_un.d_ptr);
            break;
        case DT_STRTAB:
            stringTable = resolveDynamicAddress<char>(libraryInformation, dynamicEntry->d_un.d_ptr);
            break;
        case DT_GNU_HASH:
            gnuHashTable = resolveDynamicAddress<std::uint32_t>(libraryInformation, dynamicEntry->d_un.d_ptr);
            break;
        case DT_HASH:
            systemVHashTable = resolveDynamicAddress<std::uint32_t>(libraryInformation, dynamicEntry->d_un.d_ptr);
            break;
        }
    }

    if (!symbolTable || !stringTable)
    {
        return 1;
    }

    // The dynamic section doesn't store the number of symbols, so it's derived from the hash table. Modern linkers
    // emit DT_GNU_HASH while older ones emit DT_HASH, whose second word is the number of symbols.
    auto symbolCount = 0uz;

    if (gnuHashTable)
    {
        symbolCount = getSymbolCountFromGnuHashTable(gnuHashTable);
    }
    else if (systemVHashTable)
    {
        symbolCount = systemVHashTable[1];
    }
    else
    {
        return 1;
    }

    const auto regex = std::regex{"::" + context->methodPattern};

    auto match = std::smatch{};

    for (auto index = 0uz; index < symbolCount; ++index)
    {
        const auto& symbol = symbolTable[index];

        // The ELF64 binding and type macros are identical to their ELF32 counterparts, so they work for both classes.
        const auto binding = ELF64_ST_BIND(symbol.st_info);

        // Include only functions that are defined (not imported) and exported by the library.
        const auto isExportedFunction = symbol.st_shndx != SHN_UNDEF &&
                                        (binding == STB_GLOBAL || binding == STB_WEAK) &&
                                        ELF64_ST_TYPE(symbol.st_info) == STT_FUNC;

        if (!isExportedFunction)
        {
            continue;
        }

        const auto mangledName = stringTable + symbol.st_name;
        const auto includeNamespace = true;

        auto demangledName = std::string{};

        if (!tryDemangle(mangledName, includeNamespace, demangledName) ||
            !std::regex_search(demangledName, match, regex))
        {
            continue;
        }

        const auto className = match.prefix().str();

        if (!context->classNames.contains(className))
        {
            continue;
        }

        auto& librarySymbols = context->librarySymbolsByClassName[className];

        const auto testMethodName = match[2].str();

        const auto lineNumber = std::stoi(match[1].str());

        // The symbol value is relative to the load address of the library.
        const auto address = reinterpret_cast<void*>(libraryInformation->dlpi_addr + symbol.st_value);

        // Multiple symbols could alias the same function, so they must be reported only once.
        const auto isDuplicate = std::ranges::any_of(
            librarySymbols, [address](const LibrarySymbol& librarySymbol) { return librarySymbol.address == address; }
        );

        if (isDuplicate)
        {
            continue;
        }

        librarySymbols.push_back(
            LibrarySymbol{
                .symbolName = demangledName,
                .testMethodName = testMethodName,
                .lineNumber = lineNumber,
                .address = address,
            }
        );
    }

    for (auto& [className, librarySymbols] : context->librarySymbolsByClassName)
    {
        // Sort test methods by declaration order (line number) so that the test report is easier to read.
        std::sort(
            librarySymbols.begin(), librarySymbols.end(),
            [](const auto& left, const auto& right) { return left.lineNumber < right.lineNumber; }
        );
    }

    return 1;
}

}

std::map<std::string, std::vector<TestMethodDescriptor>>
getLinuxTestClassMethods(const std::string& libraryName, const std::set<std::string>& classNames)
{
    auto context = SearchContext{
        .libraryName = libraryName,
        .classNames = classNames,
        .methodPattern = testMethodPattern,
        .libraryFound = false,
        .librarySymbolsByClassName = {},
    };

    ::dl_iterate_phdr(searchLibrary, &context);

    if (!context.libraryFound)
    {
        THROW(std::runtime_error, "Library '", libraryName, "' is not loaded in the current process");
    }

    auto testMethodByClassName = std::map<std::string, std::vector<TestMethodDescriptor>>{};

    for (const auto& [className, librarySymbols] : context.librarySymbolsByClassName)
    {
        auto& testMethodDescriptors = testMethodByClassName[className];

        for (const auto& librarySymbol : librarySymbols)
        {
            const auto address = librarySymbol.address;

            testMethodDescriptors.push_back(
                TestMethodDescriptor{
                    .testMethodName = librarySymbol.testMethodName,
                    .testMethodLineNumber = librarySymbol.lineNumber,
                    .testMethodInvoker = [address](void* object)
                    { reinterpret_cast<void (*)(void*)>(address)(object); },
                }
            );
        }
    }

    return testMethodByClassName;
}

}
#endif
