#pragma once

#include <sstream>
#include <string>

namespace dansandu::radiance::utility
{

PRALINE_EXPORT std::wstring toWideString(const char* const string);

PRALINE_EXPORT std::wstring toWideString(const std::string& string);

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
