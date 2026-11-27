#pragma once

#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/utility.hpp"

#include <array>
#include <concepts>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

namespace dansandu::radiance::representation
{

PRALINE_EXPORT std::string represent(std::nullptr_t);

PRALINE_EXPORT std::string represent(const bool value);

PRALINE_EXPORT std::string represent(const void* const value);

PRALINE_EXPORT std::string represent(const char* const value);

PRALINE_EXPORT std::string represent(const char value);

PRALINE_EXPORT std::string represent(const unsigned char value);

PRALINE_EXPORT std::string represent(const int value);

PRALINE_EXPORT std::string represent(const unsigned int value);

PRALINE_EXPORT std::string represent(const unsigned long value);

PRALINE_EXPORT std::string represent(const unsigned long long value);

PRALINE_EXPORT std::string represent(const float value);

PRALINE_EXPORT std::string represent(const double value);

PRALINE_EXPORT std::string represent(const std::string& value);

template<typename T>
std::string represent(const T* const value);

template<typename T>
std::string represent(const Tolerance<T>& tolerance);

template<typename K, typename V>
std::string represent(const std::map<K, V>& map);

template<typename T>
std::string represent(const std::set<T>& set);

template<typename T>
std::string represent(const std::vector<T>& vector);

template<typename T>
std::string represent(const T& value);

template<typename T>
std::string represent(const T* const value)
{
    return represent(static_cast<const void* const>(value));
}

template<typename T>
std::string represent(const Tolerance<T>& tolerance)
{
    auto stream = std::ostringstream{};

    stream << "Tolerance(" << represent(tolerance.target) << ", relative=" << tolerance.relative
           << ", absolute=" << tolerance.absolute << ")";

    return stream.str();
}

template<typename K, typename V>
std::string represent(const std::map<K, V>& map)
{
    auto elements = std::vector<std::string>{};

    for (const auto& entry : map)
    {
        elements.push_back(represent(entry.first) + ": " + represent(entry.second));
    }

    return "{" + dansandu::radiance::utility::join(elements) + "}";
}

template<typename T>
std::string represent(const std::set<T>& set)
{
    auto elements = std::vector<std::string>{};

    for (const auto& element : set)
    {
        elements.push_back(represent(element));
    }

    return "{" + dansandu::radiance::utility::join(elements) + "}";
}

template<typename T>
std::string represent(const std::vector<T>& vector)
{
    auto elements = std::vector<std::string>{};

    for (const auto& element : vector)
    {
        elements.push_back(represent(element));
    }

    return "[" + dansandu::radiance::utility::join(elements) + "]";
}

template<typename T, size_t N>
std::string represent(const std::array<T, N>& array)
{
    auto elements = std::vector<std::string>{};

    for (const auto& element : array)
    {
        elements.push_back(represent(element));
    }

    return "[" + dansandu::radiance::utility::join(elements) + "]";
}

template<typename T>
std::string represent(const T& value)
{
    if constexpr (dansandu::journey::utility::HasToStringMethod<T>)
    {
        return value.toString();
    }
    else if constexpr (dansandu::journey::utility::HasToStreamMethod<T>)
    {
        auto stream = std::ostringstream{};

        value.toStream(stream);

        return stream.str();
    }
    else
    {
        return "???";
    }
}

}
