#include "dansandu/radiance/representation.hpp"

#include <cctype>
#include <cwctype>
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
        static_assert(false, "unrecognized character type");
    }
}

template<typename T>
void escapeCharacter(const T character, std::wstring& result, bool doubleQuotesString)
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
std::wstring escapeString(const T* const value)
{
    if (value)
    {
        auto result = std::wstring();

        result.push_back('"');

        for (auto iterator = value; *iterator != '\0'; ++iterator)
        {
            const auto doubleQuotesString = true;
            escapeCharacter(*iterator, result, doubleQuotesString);
        }

        result.push_back('"');

        return result;
    }
    else
    {
        return represent(nullptr);
    }
}

std::wstring represent(const char* const value)
{
    return escapeString(value);
}

std::wstring represent(const wchar_t* const value)
{
    return L"L" + escapeString(value);
}

std::wstring represent(const char value)
{
    auto result = std::wstring();

    result.push_back('\'');

    const auto doubleQuotesString = false;
    escapeCharacter(value, result, doubleQuotesString);

    result.push_back('\'');

    return result;
}

std::wstring represent(const wchar_t value)
{
    auto result = std::wstring();

    result.push_back('L');

    result.push_back('\'');

    const auto doubleQuotesString = false;
    escapeCharacter(value, result, doubleQuotesString);

    result.push_back('\'');

    return result;
}

std::wstring represent(const int value)
{
    auto stream = std::wostringstream{};
    stream << value;
    return stream.str();
}

std::wstring represent(const unsigned value)
{
    auto stream = std::wostringstream{};
    stream << value << L"U";
    return stream.str();
}

std::wstring represent(const unsigned long value)
{
    auto stream = std::wostringstream{};
    stream << value << L"UL";
    return stream.str();
}

std::wstring represent(const unsigned long long value)
{
    auto stream = std::wostringstream{};
    stream << value << L"ULL";
    return stream.str();
}

std::wstring represent(const std::string& value)
{
    return escapeString(value.c_str()) + L"s";
}

std::wstring represent(const std::wstring& value)
{
    return L"L" + escapeString(value.c_str()) + L"s";
}

}
