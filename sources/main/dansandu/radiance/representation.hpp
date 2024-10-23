#pragma once

#include "dansandu/radiance/utility.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace dansandu::radiance::representation
{

PRALINE_EXPORT std::wstring represent(std::nullptr_t);

PRALINE_EXPORT std::wstring represent(const bool value);

PRALINE_EXPORT std::wstring represent(const void* const value);

PRALINE_EXPORT std::wstring represent(const char* const value);

PRALINE_EXPORT std::wstring represent(const wchar_t* const value);

PRALINE_EXPORT std::wstring represent(const char value);

PRALINE_EXPORT std::wstring represent(const unsigned char value);

PRALINE_EXPORT std::wstring represent(const wchar_t value);

PRALINE_EXPORT std::wstring represent(const int value);

PRALINE_EXPORT std::wstring represent(const unsigned int value);

PRALINE_EXPORT std::wstring represent(const unsigned long value);

PRALINE_EXPORT std::wstring represent(const unsigned long long value);

PRALINE_EXPORT std::wstring represent(const float value);

PRALINE_EXPORT std::wstring represent(const double value);

PRALINE_EXPORT std::wstring represent(const std::string& value);

PRALINE_EXPORT std::wstring represent(const std::wstring& value);

template<typename T>
std::wstring represent(const T* const value)
{
    return represent(static_cast<const void* const>(value));
}

template<typename T>
std::wstring represent(const T& value)
{
    return L"???";
}

template<typename T>
std::wstring represent(const std::vector<T>& vector)
{
    auto elements = std::vector<std::wstring>{};

    for (const auto& element : vector)
    {
        elements.push_back(represent(element));
    }

    return L"[" + dansandu::radiance::utility::join(elements) + L"]";
}

template<typename K, typename V>
std::wstring represent(const std::map<K, V>& map)
{
    auto elements = std::vector<std::wstring>{};

    for (const auto& entry : map)
    {
        elements.push_back(represent(entry.first) + L": " + represent(entry.second));
    }

    return L"{" + dansandu::radiance::utility::join(elements) + L"}";
}

template<typename T>
std::wstring represent(const std::set<T>& set)
{
    auto elements = std::vector<std::wstring>{};

    for (const auto& element : set)
    {
        elements.push_back(represent(element));
    }

    return L"{" + dansandu::radiance::utility::join(elements) + L"}";
}

}
