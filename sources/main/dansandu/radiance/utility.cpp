#include "dansandu/radiance/utility.hpp"

#include <sstream>

namespace dansandu::radiance::utility
{

std::wstring toWideString(const char* const string)
{
    auto stream = std::wostringstream{};
    stream << string;
    return stream.str();
}

std::wstring toWideString(const std::string& string)
{
    return toWideString(string.c_str());
}

}
