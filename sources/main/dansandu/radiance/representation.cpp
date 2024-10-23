#include "dansandu/radiance/representation.hpp"

#include <cctype>
#include <cmath>
#include <cwctype>
#include <iomanip>
#include <sstream>
#include <type_traits>

namespace dansandu::radiance::representation
{

std::wstring represent(std::nullptr_t)
{
    return L"nullptr";
}

std::wstring represent(const bool value)
{
    return value ? L"true" : L"false";
}

std::wstring represent(const void* const value)
{
    if (value)
    {
        auto stream = std::wostringstream{};
        stream << "0x" << value;
        return stream.str();
    }
    else
    {
        return represent(nullptr);
    }
}

template<typename T>
static bool isPrintable(const T character)
{
    using CharacterType = std::decay_t<decltype(character)>;

    if constexpr (std::is_same_v<CharacterType, char>)
    {
        return std::isprint(static_cast<unsigned char>(character));
    }
    else if constexpr (std::is_same_v<CharacterType, wchar_t>)
    {
        return std::iswprint(static_cast<unsigned char>(character));
    }
    else
    {
        static_assert(false, "isPrintable character type is non-exhaustive");
    }
}

template<typename T>
void escapeCharacter(const T character, const bool doubleQuotesString, std::wstring& result)
{
    if (doubleQuotesString && character == '"')
    {
        result.push_back('\\');
        result.push_back('"');
    }
    else if (!doubleQuotesString && character == '\'')
    {
        result.push_back('\\');
        result.push_back('\'');
    }
    else if (character == '\n')
    {
        result.push_back('\\');
        result.push_back('n');
    }
    else if (character == '\t')
    {
        result.push_back('\\');
        result.push_back('t');
    }
    else if (character == '\\')
    {
        result.push_back('\\');
        result.push_back('\\');
    }
    else if (isPrintable(character))
    {
        result.push_back(character);
    }
    else
    {
        const auto hex = "0123456789ABCDEF";
        result.push_back('\\');
        result.push_back('x');
        result.push_back(hex[character & 0xF]);
        result.push_back(hex[(character >> 4) & 0xF]);
    }
}

template<typename T>
void escapeString(const T* const value, std::wstring& result)
{
    result.push_back('"');

    for (auto iterator = value; iterator && *iterator != '\0'; ++iterator)
    {
        const auto doubleQuotesString = true;

        escapeCharacter(*iterator, doubleQuotesString, result);
    }

    result.push_back('"');
}

std::wstring represent(const char* const value)
{
    if (value)
    {
        auto result = std::wstring{};

        escapeString(value, result);

        return result;
    }
    else
    {
        return represent(nullptr);
    }
}

std::wstring represent(const wchar_t* const value)
{
    if (value)
    {
        auto result = std::wstring{};

        result.push_back(L'L');

        escapeString(value, result);

        return result;
    }
    else
    {
        return represent(nullptr);
    }
}

std::wstring represent(const char value)
{
    auto result = std::wstring();

    result.push_back('\'');

    const auto doubleQuotesString = false;

    escapeCharacter(value, doubleQuotesString, result);

    result.push_back('\'');

    return result;
}

std::wstring represent(const unsigned char value)
{
    auto result = std::wstring();

    const auto hex = "0123456789ABCDEF";
    result.push_back('0');
    result.push_back('x');
    result.push_back(hex[value & 0xF]);
    result.push_back(hex[(value >> 4) & 0xF]);

    return result;
}

std::wstring represent(const wchar_t value)
{
    auto result = std::wstring();

    result.push_back('L');

    result.push_back('\'');

    const auto doubleQuotesString = false;

    escapeCharacter(value, doubleQuotesString, result);

    result.push_back('\'');

    return result;
}

std::wstring represent(const int value)
{
    auto stream = std::wostringstream{};
    stream << value;
    return stream.str();
}

std::wstring represent(const unsigned int value)
{
    auto stream = std::wostringstream{};
    stream << value << L'u';
    return stream.str();
}

std::wstring represent(const unsigned long value)
{
    auto stream = std::wostringstream{};
    stream << value << L"ul";
    return stream.str();
}

std::wstring represent(const unsigned long long value)
{
    auto stream = std::wostringstream{};
    stream << value << L"ull";
    return stream.str();
}

std::wstring represent(const float value)
{
    auto stream = std::wostringstream{};

    stream << std::fixed << std::setprecision(6) << value << L'f';

    return stream.str();
}

std::wstring represent(const double value)
{
    auto stream = std::wostringstream{};

    stream << std::fixed << std::setprecision(6) << value;

    return stream.str();
}

std::wstring represent(const std::string& value)
{
    auto result = std::wstring{};

    escapeString(value.c_str(), result);

    result.push_back(L's');

    return result;
}

std::wstring represent(const std::wstring& value)
{
    auto result = std::wstring{};

    result.push_back(L'L');

    escapeString(value.c_str(), result);

    result.push_back(L's');

    return result;
}

}
