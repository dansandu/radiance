#pragma once

#include <filesystem>
#include <optional>
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

PRALINE_EXPORT std::wstring toWideString(const char* const string);

PRALINE_EXPORT std::wstring toWideString(const std::string& string);

PRALINE_EXPORT std::wstring toWideString(std::wstring string);

PRALINE_EXPORT std::optional<std::string> getEnvironmentVariable(const std::string& variable);

PRALINE_EXPORT std::wstring readFile(const std::filesystem::path& filePath);

PRALINE_EXPORT std::wstring removeCarriage(std::wstring text);

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
