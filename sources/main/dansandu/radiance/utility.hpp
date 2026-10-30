#pragma once

#include "dansandu/journey/common.hpp"
#include "dansandu/radiance/common.hpp"

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

PRALINE_EXPORT std::string highlightText(const std::string& text, const TextHighlight textHighlight);

PRALINE_EXPORT std::string readFile(const std::filesystem::path& filePath);

PRALINE_EXPORT std::string removeCarriage(std::string text);

PRALINE_EXPORT std::vector<Log> journeyLogsToRadianceLogs(const std::vector<dansandu::journey::LogEntry>& logs);

PRALINE_EXPORT std::string normalizePath(const std::string& root, const std::string& path);

// Checks whether both paths refer to the same existing file, regardless of how each path is spelled (e.g. relative,
// absolute or through a symbolic link). Returns false if either path doesn't exist.
PRALINE_EXPORT bool isSameFile(const std::filesystem::path& firstPath, const std::filesystem::path& secondPath);

template<typename Iterable>
auto join(const Iterable& iterable, const std::string separator = ", ")
{
    auto stream = std::ostringstream{};
    for (const auto& element : iterable)
    {
        stream << element << separator;
    }

    auto result = stream.str();
    result.erase(result.end() - std::min(separator.size(), result.size()), result.end());

    return result;
}

}
