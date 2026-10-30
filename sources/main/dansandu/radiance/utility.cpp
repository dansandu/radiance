#include "dansandu/radiance/utility.hpp"
#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/utility.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>

using dansandu::journey::utility::replaceBackSlashes;
using dansandu::journey::utility::tryGetRelativePath;

namespace dansandu::radiance::utility
{

std::string highlightText(const std::string& text, const TextHighlight textHighlight)
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
        return "\x1B[31m" + text + "\x1B[0m";
    case TextHighlight::Green:
        return "\x1B[32m" + text + "\x1B[0m";
    case TextHighlight::Yellow:
        return "\x1B[33m" + text + "\x1B[0m";
    case TextHighlight::Blue:
        return "\x1B[34m" + text + "\x1B[0m";
    case TextHighlight::Magenta:
        return "\x1B[35m" + text + "\x1B[0m";
    default:
        THROW(std::logic_error, "Unknown text highlight");
    }
}

std::string readFile(const std::filesystem::path& filePath)
{
    auto file = std::ifstream{filePath, std::ios_base::binary};
    file >> std::noskipws;
    auto stream = std::ostringstream{};
    stream << file.rdbuf();
    return stream.str();
}

std::string removeCarriage(std::string text)
{
    text.erase(std::remove(text.begin(), text.end(), '\r'), text.end());
    return text;
}

std::vector<Log> journeyLogsToRadianceLogs(const std::vector<dansandu::journey::LogEntry>& logs)
{
    auto result = std::vector<Log>{};

    for (const auto& log : logs)
    {
        result.push_back(
            Log{
                .level = log.level,
                .message = log.message,
            }
        );
    }

    return result;
}

std::string normalizePath(const std::string& root, const std::string& path)
{
    auto normalizedPath = std::string{};

    tryGetRelativePath(replaceBackSlashes(path), replaceBackSlashes(root), normalizedPath);

    return normalizedPath;
}

bool isSameFile(const std::filesystem::path& firstPath, const std::filesystem::path& secondPath)
{
    auto errorCode = std::error_code{};

    const auto sameFile = std::filesystem::equivalent(firstPath, secondPath, errorCode);

    return !errorCode && sameFile;
}

}
