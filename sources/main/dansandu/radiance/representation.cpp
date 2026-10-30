#include "dansandu/radiance/representation.hpp"

#include <cctype>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <type_traits>

namespace dansandu::radiance::representation
{

std::string represent(std::nullptr_t)
{
    return "nullptr";
}

std::string represent(const bool value)
{
    return value ? "true" : "false";
}

std::string represent(const void* const value)
{
    if (value)
    {
        auto stream = std::ostringstream{};
        stream << "0x" << std::setfill('0') << std::setw(16) << std::hex << std::uppercase
               << reinterpret_cast<std::uintptr_t>(value);
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
    else
    {
        static_assert(false, "isPrintable character type is non-exhaustive");
    }
}

template<typename T>
void escapeCharacter(const T character, const bool doubleQuotesString, std::string& result)
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
        result.push_back(hex[(character >> 4) & 0x0F]);
        result.push_back(hex[character & 0x0F]);
    }
}

template<typename T>
void escapeString(const T* const value, std::string& result)
{
    result.push_back('"');

    for (auto iterator = value; iterator && *iterator != '\0'; ++iterator)
    {
        const auto doubleQuotesString = true;

        escapeCharacter(*iterator, doubleQuotesString, result);
    }

    result.push_back('"');
}

std::string represent(const char* const value)
{
    if (value)
    {
        auto result = std::string{};

        escapeString(value, result);

        return result;
    }
    else
    {
        return represent(nullptr);
    }
}

std::string represent(const char value)
{
    auto result = std::string();

    result.push_back('\'');

    const auto doubleQuotesString = false;

    escapeCharacter(value, doubleQuotesString, result);

    result.push_back('\'');

    return result;
}

std::string represent(const unsigned char value)
{
    auto result = std::string();

    const auto hex = "0123456789ABCDEF";
    result.push_back('0');
    result.push_back('x');
    result.push_back(hex[(value >> 4) & 0x0F]);
    result.push_back(hex[value & 0x0F]);

    return result;
}

std::string represent(const int value)
{
    auto stream = std::ostringstream{};
    stream << value;
    return stream.str();
}

std::string represent(const unsigned int value)
{
    auto stream = std::ostringstream{};
    stream << value << 'u';
    return stream.str();
}

std::string represent(const unsigned long value)
{
    auto stream = std::ostringstream{};
    stream << value << "ul";
    return stream.str();
}

std::string represent(const unsigned long long value)
{
    auto stream = std::ostringstream{};
    stream << value << "ull";
    return stream.str();
}

std::string represent(const float value)
{
    auto stream = std::ostringstream{};

    stream << std::fixed << std::setprecision(6) << value << 'f';

    return stream.str();
}

std::string represent(const double value)
{
    auto stream = std::ostringstream{};

    stream << std::fixed << std::setprecision(6) << value;

    return stream.str();
}

std::string represent(const std::string& value)
{
    auto result = std::string{};

    escapeString(value.c_str(), result);

    result.push_back('s');

    return result;
}

}
