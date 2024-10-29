#pragma once

#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/representation.hpp"
#include "dansandu/radiance/utility.hpp"

#include <concepts>
#include <string>
#include <type_traits>

namespace dansandu::radiance::binding
{

template<typename T>
struct ArgumentBinding
{
    const T* argument;
    std::wstring representation;
};

template<typename T, typename U>
auto operator==(const ArgumentBinding<T>& first, const ArgumentBinding<U>& second)
{
    return std::make_pair(BinaryAssertion{.firstRepresentation = first.representation,
                                          .secondRepresentation = second.representation,
                                          .operation = "=="},
                          static_cast<bool>(*first.argument == *second.argument));
}

template<typename T, typename U>
auto operator!=(const ArgumentBinding<T>& first, const ArgumentBinding<U>& second)
{
    return std::make_pair(
        BinaryAssertion{
            .firstRepresentation = first.representation,
            .secondRepresentation = second.representation,
            .operation = "!=",
        },
        static_cast<bool>(*first.argument != *second.argument));
}

template<typename T, typename U>
auto operator<(const ArgumentBinding<T>& first, const ArgumentBinding<U>& second)
{
    return std::make_pair(
        BinaryAssertion{
            .firstRepresentation = first.representation,
            .secondRepresentation = second.representation,
            .operation = "<",
        },
        static_cast<bool>(*first.argument < *second.argument));
}

template<typename T, typename U>
auto operator<=(const ArgumentBinding<T>& first, const ArgumentBinding<U>& second)
{
    return std::make_pair(
        BinaryAssertion{
            .firstRepresentation = first.representation,
            .secondRepresentation = second.representation,
            .operation = "<=",
        },
        static_cast<bool>(*first.argument <= *second.argument));
}

template<typename T, typename U>
auto operator>(const ArgumentBinding<T>& first, const ArgumentBinding<U>& second)
{
    return std::make_pair(
        BinaryAssertion{
            .firstRepresentation = first.representation,
            .secondRepresentation = second.representation,
            .operation = ">",
        },
        static_cast<bool>(*first.argument > *second.argument));
}

template<typename T, typename U>
auto operator>=(const ArgumentBinding<T>& first, const ArgumentBinding<U>& second)
{
    return std::make_pair(
        BinaryAssertion{
            .firstRepresentation = first.representation,
            .secondRepresentation = second.representation,
            .operation = ">=",
        },
        static_cast<bool>(*first.argument >= *second.argument));
}

struct ArgumentBinder
{
};

template<typename T>
concept ToStringable = requires(const T value) {
    {
        value.toString()
    } -> std::convertible_to<std::string>;
} || requires(const T value) {
    {
        value.toString()
    } -> std::convertible_to<std::wstring>;
};

template<typename T>
std::wstring stringify(const T& value)
{
    if constexpr (ToStringable<T>)
    {
        return dansandu::radiance::utility::toWideString(value.toString());
    }
    else
    {
        return dansandu::radiance::representation::represent(value);
    }
}

template<typename T>
auto operator<<(const ArgumentBinder, const T& argument)
{
    return ArgumentBinding<T>{.argument = &argument, .representation = stringify(argument)};
}

template<typename T>
auto operator>>(const T& argument, const ArgumentBinder binder)
{
    return binder << argument;
}

template<typename T>
auto operator>>(const ArgumentBinding<T>& binding, const ArgumentBinder)
{
    return std::make_pair(
        UnaryAssertion{
            .representation = binding.representation,
        },
        static_cast<bool>(*binding.argument));
}

}
