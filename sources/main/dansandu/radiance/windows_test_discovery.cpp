#if defined(_WIN32)
#include "dansandu/radiance/windows_test_discovery.hpp"
#include "dansandu/journey/exception.hpp"
#include "dansandu/radiance/macro.hpp"
#include "dansandu/radiance/test_class_descriptor.hpp"
#include "dansandu/radiance/utility.hpp"

#include <algorithm>
#include <cstdint>
#include <map>
#include <regex>
#include <set>
#include <string>
#include <string_view>
#include <vector>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>

#include <dbghelp.h>
#include <psapi.h>

#pragma comment(lib, "dbghelp.lib")
#pragma comment(lib, "psapi.lib")

using dansandu::radiance::macro::testMethodPattern;
using dansandu::radiance::test_class_descriptor::TestMethodDescriptor;
using dansandu::radiance::utility::isSameFile;

namespace dansandu::radiance::windows_test_discovery
{

namespace
{

struct LibrarySymbol
{
    std::string symbolName;
    std::string testMethodName;
    int lineNumber;
    void* address;
};

std::string getModulePath(const HMODULE module)
{
    auto path = std::string(MAX_PATH, '\0');

    while (true)
    {
        const auto length = ::GetModuleFileNameA(module, path.data(), static_cast<DWORD>(path.size()));

        if (length == 0)
        {
            return {};
        }

        if (length < path.size())
        {
            path.resize(length);
            return path;
        }

        // The path was truncated, so retry with a larger buffer.
        path.resize(path.size() * 2);
    }
}

HMODULE findModule(const std::string& libraryName)
{
    const auto process = ::GetCurrentProcess();

    auto modules = std::vector<HMODULE>(256);

    while (true)
    {
        auto bytesNeeded = DWORD{};

        const auto bytesAvailable = static_cast<DWORD>(modules.size() * sizeof(HMODULE));

        if (!::EnumProcessModules(process, modules.data(), bytesAvailable, &bytesNeeded))
        {
            return nullptr;
        }

        if (bytesNeeded <= bytesAvailable)
        {
            modules.resize(bytesNeeded / sizeof(HMODULE));
            break;
        }

        // Modules might be loaded between calls, so loop until the buffer is large enough.
        modules.resize(bytesNeeded / sizeof(HMODULE));
    }

    for (const auto module : modules)
    {
        if (isSameFile(getModulePath(module), libraryName))
        {
            return module;
        }
    }

    return nullptr;
}

// Transforms a decorated MSVC name such as `?dansandu_radiance_test_method_12_foo@IntegerTest@scenario@@QEAAXXZ` into
// `scenario::IntegerTest::dansandu_radiance_test_method_12_foo()`, which matches the format produced by the Itanium ABI
// demangler on Linux.
bool tryUndecorate(const char* const decorated, std::string& undecorated)
{
    // Only decorated C++ names start with a question mark.
    if (decorated[0] != '?')
    {
        return false;
    }

    constexpr auto flags = UNDNAME_NO_LEADING_UNDERSCORES | UNDNAME_NO_MS_KEYWORDS | UNDNAME_NO_FUNCTION_RETURNS |
                           UNDNAME_NO_ALLOCATION_MODEL | UNDNAME_NO_ALLOCATION_LANGUAGE | UNDNAME_NO_ACCESS_SPECIFIERS |
                           UNDNAME_NO_THROW_SIGNATURES | UNDNAME_NO_MEMBER_TYPE | UNDNAME_NO_RETURN_UDT_MODEL |
                           UNDNAME_NO_THISTYPE;

    char buffer[4096];

    const auto length = ::UnDecorateSymbolName(decorated, buffer, static_cast<DWORD>(sizeof(buffer)), flags);

    if (length == 0)
    {
        return false;
    }

    auto result = std::string_view{buffer, length};

    // Functions without parameters are undecorated with an explicit `void` parameter list.
    constexpr auto voidParameters = std::string_view{"(void)"};

    if (result.ends_with(voidParameters))
    {
        result.remove_suffix(voidParameters.size());
        undecorated = std::string{result} + "()";
    }
    else
    {
        undecorated = result;
    }

    return true;
}

std::map<std::string, std::vector<LibrarySymbol>>
findLibrarySymbols(const std::string& libraryName, const std::set<std::string>& classNames)
{
    auto librarySymbolsByClassName = std::map<std::string, std::vector<LibrarySymbol>>{};

    const auto module = findModule(libraryName);

    if (!module)
    {
        THROW(std::runtime_error, "Library '", libraryName, "' is not loaded in the current process");
    }

    // The module handle is the base address at which the image was loaded.
    const auto base = reinterpret_cast<const std::uint8_t*>(module);

    const auto dosHeader = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);

    if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE)
    {
        return librarySymbolsByClassName;
    }

    const auto ntHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(base + dosHeader->e_lfanew);

    if (ntHeaders->Signature != IMAGE_NT_SIGNATURE)
    {
        return librarySymbolsByClassName;
    }

    const auto& exportDataDirectory = ntHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];

    if (exportDataDirectory.VirtualAddress == 0 || exportDataDirectory.Size == 0)
    {
        return librarySymbolsByClassName;
    }

    const auto exportDirectory =
        reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(base + exportDataDirectory.VirtualAddress);

    const auto functions = reinterpret_cast<const DWORD*>(base + exportDirectory->AddressOfFunctions);
    const auto names = reinterpret_cast<const DWORD*>(base + exportDirectory->AddressOfNames);
    const auto ordinals = reinterpret_cast<const WORD*>(base + exportDirectory->AddressOfNameOrdinals);

    const auto regex = std::regex{std::string{"::"} + testMethodPattern};

    auto match = std::smatch{};

    for (auto i = 0uz; i < exportDirectory->NumberOfNames; ++i)
    {
        const auto ordinal = ordinals[i];

        if (ordinal >= exportDirectory->NumberOfFunctions)
        {
            continue;
        }

        const auto functionRva = functions[ordinal];

        // Forwarded exports point inside the export directory to a string naming another module's export, so they
        // don't refer to code in this library.
        const auto isForwarded = functionRva >= exportDataDirectory.VirtualAddress &&
                                 functionRva < exportDataDirectory.VirtualAddress + exportDataDirectory.Size;

        if (functionRva == 0 || isForwarded)
        {
            continue;
        }

        const auto decorated = reinterpret_cast<const char*>(base + names[i]);

        auto undecorated = std::string{};

        if (!tryUndecorate(decorated, undecorated) || !std::regex_search(undecorated, match, regex))
        {
            continue;
        }

        const auto className = match.prefix().str();

        if (!classNames.contains(className))
        {
            continue;
        }

        auto& librarySymbols = librarySymbolsByClassName[className];

        const auto testMethodName = match[2].str();

        const auto lineNumber = std::stoi(match[1].str());

        // The export address is relative to the load address of the library.
        const auto address = const_cast<void*>(static_cast<const void*>(base + functionRva));

        // Multiple exported names could alias the same function, so they must be reported only once.
        const auto isDuplicate = std::ranges::any_of(
            librarySymbols, [address](const LibrarySymbol& librarySymbol) { return librarySymbol.address == address; }
        );

        if (isDuplicate)
        {
            continue;
        }

        librarySymbols.push_back(
            LibrarySymbol{
                .symbolName = undecorated,
                .testMethodName = testMethodName,
                .lineNumber = lineNumber,
                .address = address,
            }
        );
    }

    for (auto& [className, librarySymbols] : librarySymbolsByClassName)
    {
        // Sort test methods by declaration order (line number) so that the test report is easier to read.
        std::sort(
            librarySymbols.begin(), librarySymbols.end(),
            [](const auto& left, const auto& right) { return left.lineNumber < right.lineNumber; }
        );
    }

    return librarySymbolsByClassName;
}

void invokeTestMethod(void* const address, void* const object)
{
#if defined(_M_IX86)
    // On 32-bit x86, member functions use the __thiscall convention which passes `this` in ECX and has the callee
    // clean up the stack. The __fastcall convention passes its first two arguments in ECX and EDX and also has the
    // callee clean up the stack, so it can be used to call a parameterless member function.
    reinterpret_cast<void(__fastcall*)(void*, void*)>(address)(object, nullptr);
#else
    // On 64-bit targets there is a single calling convention and `this` is passed as the first argument.
    reinterpret_cast<void (*)(void*)>(address)(object);
#endif
}

}

std::map<std::string, std::vector<TestMethodDescriptor>>
getWindowsTestClassMethods(const std::string& libraryName, const std::set<std::string>& classNames)
{
    const auto librarySymbolsByClassName = findLibrarySymbols(libraryName, classNames);

    auto testMethodByClassName = std::map<std::string, std::vector<TestMethodDescriptor>>{};

    for (const auto& [className, librarySymbols] : librarySymbolsByClassName)
    {
        auto& testMethodDescriptors = testMethodByClassName[className];

        for (const auto& librarySymbol : librarySymbols)
        {
            const auto address = librarySymbol.address;

            testMethodDescriptors.push_back(
                TestMethodDescriptor{
                    .testMethodName = librarySymbol.testMethodName,
                    .testMethodLineNumber = librarySymbol.lineNumber,
                    .testMethodInvoker = [address](void* object) { invokeTestMethod(address, object); },
                }
            );
        }
    }

    return testMethodByClassName;
}

}
#endif
