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

std::vector<Log> journeyLogsToRadianceLogs(const std::vector<dansandu::journey::LogEntry>& logs)
{
    auto result = std::vector<Log>{};

    for (const auto& log : logs)
    {
        result.push_back(Log{
            .level = log.level,
            .message = log.message,
        });
    }

    return result;
}

}
