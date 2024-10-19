#pragma once

#include <string>

namespace dansandu::radiance::representation
{

PRALINE_EXPORT std::wstring represent(std::nullptr_t);

PRALINE_EXPORT std::wstring represent(const bool value);

PRALINE_EXPORT std::wstring represent(const void* const value);

PRALINE_EXPORT std::wstring represent(const char* const value);

PRALINE_EXPORT std::wstring represent(const wchar_t* const value);

PRALINE_EXPORT std::wstring represent(const std::wstring& value);

PRALINE_EXPORT std::wstring represent(const char value);

PRALINE_EXPORT std::wstring represent(const wchar_t value);

PRALINE_EXPORT std::wstring represent(const int value);

PRALINE_EXPORT std::wstring represent(const unsigned value);

PRALINE_EXPORT std::wstring represent(const unsigned long value);

PRALINE_EXPORT std::wstring represent(const unsigned long long value);

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

}
