#pragma once

#include <filesystem>
#include <sstream>
#include <string>

namespace dansandu::radiance::utility
{

enum class TextHighlight
{
    None,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
};

PRALINE_EXPORT std::wstring highlightText(const std::wstring& text, const TextHighlight textHighlight);

PRALINE_EXPORT std::wstring readFile(const std::filesystem::path& filePath);

PRALINE_EXPORT std::wstring removeCarriage(std::wstring text);

PRALINE_EXPORT bool tryDemangle(const std::string& symbol, std::string& output);

template<typename Exception>
std::string getExceptionTypeName()
{
    const auto mangled = typeid(Exception).name();
    auto demangled = std::string{mangled};
    tryDemangle(mangled, demangled);
    return demangled;
}

template<typename Exception>
std::string getExceptionTypeName(const Exception& exception)
{
    const auto mangled = typeid(exception).name();
    auto demangled = std::string{mangled};
    tryDemangle(mangled, demangled);
    return demangled;
}

template<typename Iterable>
auto join(const Iterable& iterable, const std::wstring separator = L", ")
{
    auto stream = std::wostringstream{};
    for (const auto& element : iterable)
    {
        stream << element << separator;
    }

    auto result = stream.str();
    result.erase(result.end() - std::min(separator.size(), result.size()), result.end());

    return result;
}

}
