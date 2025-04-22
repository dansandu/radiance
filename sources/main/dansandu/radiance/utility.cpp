#include "dansandu/radiance/utility.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>

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
        throw std::logic_error{"unknown text highlight"};
    }
}

std::optional<std::string> getEnvironmentVariable(const std::string& variable)
{
    size_t requiredSize;

    getenv_s(&requiredSize, nullptr, 0, variable.c_str());
    if (requiredSize == 0)
    {
        return {};
    }

    const auto value = std::make_unique<char[]>(requiredSize);

    getenv_s(&requiredSize, value.get(), requiredSize, variable.c_str());

    return std::optional<std::string>{std::in_place, value.get()};
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

}
