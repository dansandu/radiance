#include "dansandu/radiance/utility.hpp"
#include "dansandu/journey/exception.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>

#if defined(__GNUG__)
#include <cxxabi.h>
#endif

namespace dansandu::radiance::utility
{

std::wstring highlightText(const std::wstring& text, const TextHighlight textHighlight)
{
    if (text.empty())
    {
        return text;
    }

    switch (textHighlight)
    {
    case TextHighlight::None:
        return text;
    case TextHighlight::Red:
        return L"\x1B[31m" + text + L"\x1B[0m";
    case TextHighlight::Green:
        return L"\x1B[32m" + text + L"\x1B[0m";
    case TextHighlight::Yellow:
        return L"\x1B[33m" + text + L"\x1B[0m";
    case TextHighlight::Blue:
        return L"\x1B[34m" + text + L"\x1B[0m";
    case TextHighlight::Magenta:
        return L"\x1B[35m" + text + L"\x1B[0m";
    default:
        THROW(std::logic_error, "Unknown text highlight");
    }
}

std::wstring readFile(const std::filesystem::path& filePath)
{
    auto file = std::wifstream{filePath, std::ios_base::binary};
    file >> std::noskipws;
    auto stream = std::wostringstream{};
    stream << file.rdbuf();
    return stream.str();
}

std::wstring removeCarriage(std::wstring text)
{
    text.erase(std::remove(text.begin(), text.end(), L'\r'), text.end());
    return text;
}

bool tryDemangle(const std::string& symbol, std::string& output)
{
#if defined(__GNUG__)
    char* const buffer = nullptr;
    size_t* const length = nullptr;
    auto status = -4;
    const auto demangled =
        std::unique_ptr<char, void (*)(void*)>{abi::__cxa_demangle(symbol.c_str(), buffer, length, &status), std::free};
    if (status == 0)
    {
        output = demangled.get();
        return true;
    }
    return false;
#elif defined(_MSC_VER)
    const auto classPrefix = std::string_view{"class "};
    if (symbol.starts_with(classPrefix))
    {
        output = std::string{symbol.cbegin() + classPrefix.size(), symbol.cend()};
        return true;
    }
    const auto structPrefix = std::string_view{"struct "};
    if (symbol.starts_with(structPrefix))
    {
        output = std::string{symbol.cbegin() + structPrefix.size(), symbol.cend()};
        return true;
    }
    return true;
#else
#error "Unkown platform"
#endif
}

}
