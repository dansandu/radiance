#if defined(__linux__)
#include "dansandu/radiance/library.hpp"
#include "dansandu/journey/utility.hpp"

#include <algorithm>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include <cxxabi.h>
#include <elf.h>
#include <link.h>

using dansandu::journey::utility::tryDemangle;

namespace dansandu::radiance::library
{

namespace
{

// Safely compute the exact number of symbols from the DT_GNU_HASH table in memory
size_t get_sym_count_from_gnu_hash(const uint32_t* gnu_hash)
{
    uint32_t nbuckets = gnu_hash[0];
    uint32_t symoffset = gnu_hash[1];
    uint32_t bloom_size = gnu_hash[2];
    // gnu_hash[3] is bloom_shift

    // Skip the 4 header words + bloom filter (64-bit words on 64-bit ELF)
    const auto* bloom = reinterpret_cast<const ElfW(Addr)*>(gnu_hash + 4);
    const auto* buckets = reinterpret_cast<const uint32_t*>(bloom + bloom_size);
    const auto* chain = buckets + nbuckets;

    // Find the highest symbol index referenced by any bucket
    uint32_t max_sym = 0;
    for (uint32_t i = 0; i < nbuckets; ++i)
    {
        if (buckets[i] > max_sym)
        {
            max_sym = buckets[i];
        }
    }

    if (max_sym < symoffset)
    {
        return symoffset;
    }

    // Walk the hash chain for the highest bucket until the stop bit (bit 0 == 1) is found
    while ((chain[max_sym - symoffset] & 1) == 0)
    {
        ++max_sym;
    }

    return max_sym + 1;
}

struct SearchContext
{
    std::string libraryName;
    std::string className;
    std::string methodPrefix;
    std::vector<LibrarySymbol> symbols;
};

int phdrCallback(struct dl_phdr_info* info, size_t /*size*/, void* data)
{
    const auto context = static_cast<SearchContext*>(data);

    // info->dlpi_name is empty ("") for the main executable itself.
    if (!info->dlpi_name || std::strstr(info->dlpi_name, context->libraryName.c_str()) == nullptr)
    {
        // Continue iterating through loaded libraries.
        return 0;
    }

    // context->found = true;

    // Locate the PT_DYNAMIC segment in the loaded library's program headers
    const ElfW(Dyn)* dyn = nullptr;

    for (ElfW(Half) i = 0; i < info->dlpi_phnum; ++i)
    {
        if (info->dlpi_phdr[i].p_type == PT_DYNAMIC)
        {
            dyn = reinterpret_cast<const ElfW(Dyn)*>(info->dlpi_addr + info->dlpi_phdr[i].p_vaddr);
            break;
        }
    }

    if (!dyn)
    {
        return 1;
    }

    const ElfW(Sym)* symtab = nullptr;
    const char* strtab = nullptr;
    const uint32_t* gnu_hash = nullptr;
    const uint32_t* sysv_hash = nullptr;

    // Extract symbol table, string table, and hash table addresses from PT_DYNAMIC
    for (; dyn->d_tag != DT_NULL; ++dyn)
    {
        switch (dyn->d_tag)
        {
        case DT_SYMTAB:
            symtab = reinterpret_cast<const ElfW(Sym)*>(dyn->d_un.d_ptr);
            break;
        case DT_STRTAB:
            strtab = reinterpret_cast<const char*>(dyn->d_un.d_ptr);
            break;
        case DT_GNU_HASH:
            gnu_hash = reinterpret_cast<const uint32_t*>(dyn->d_un.d_ptr);
            break;
        case DT_HASH:
            sysv_hash = reinterpret_cast<const uint32_t*>(dyn->d_un.d_ptr);
            break;
        }
    }

    if (!symtab || !strtab)
    {
        return 1;
    }

    // Determine exact symbol count (modern GCC uses DT_GNU_HASH; legacy uses DT_HASH)
    auto sym_count = 0uz;

    if (gnu_hash)
    {
        sym_count = get_sym_count_from_gnu_hash(gnu_hash);
    }
    else if (sysv_hash)
    {
        sym_count = sysv_hash[1]; // nchain is the 2nd word in SysV DT_HASH
    }
    else
    {
        return 1;
    }

    const auto symbolFilter =
        context->methodPrefix.empty() ? context->className : context->className + "::" + context->methodPrefix;

    for (auto i = 0uz; i < sym_count; ++i)
    {
        const ElfW(Sym) & sym = symtab[i];

        unsigned char bind = ELF64_ST_BIND(sym.st_info);

        // Include only defined (not imported) GLOBAL or WEAK exported symbols
        if (sym.st_shndx != SHN_UNDEF && (bind == STB_GLOBAL || bind == STB_WEAK) &&
            ELF64_ST_TYPE(sym.st_info) == STT_FUNC)
        {
            const auto mangled = strtab + sym.st_name;
            const auto includeNamespace = true;

            auto demangled = std::string{};

            if (!tryDemangle(mangled, includeNamespace, demangled) || !demangled.starts_with(symbolFilter))
            {
                continue;
            }

            // The symbol value is relative to the load address of the library.
            const auto pointer = reinterpret_cast<void*>(info->dlpi_addr + sym.st_value);

            // Constructors and destructors have multiple aliased symbols (e.g. C1/C2 and D1/D2) that share the same
            // address and demangled name, so they must be reported only once.
            const auto isDuplicate = std::ranges::any_of(context->symbols, [pointer](const LibrarySymbol& symbol)
                                                         { return symbol.pointer == pointer; });

            if (isDuplicate)
            {
                continue;
            }

            context->symbols.push_back(LibrarySymbol{.name = demangled, .pointer = pointer});
        }
    }

    // Stop iterating once target library is processed
    return 1;
}

}

std::vector<LibrarySymbol> getClassMethods(const std::string& libraryName, const std::string& className,
                                           const std::string& methodPrefix)
{
    auto context = SearchContext{
        .libraryName = libraryName,
        .className = className,
        .methodPrefix = methodPrefix,
        .symbols = {},
    };

    ::dl_iterate_phdr(phdrCallback, &context);

    std::sort(context.symbols.begin(), context.symbols.end(),
              [](const auto& left, const auto& right) { return left.name < right.name; });

    return context.symbols;
};

void invokeVoidClassMethod(const LibrarySymbol& symbol, void* const object)
{
    reinterpret_cast<void (*)(void*)>(symbol.pointer)(object);
}

}
#endif
