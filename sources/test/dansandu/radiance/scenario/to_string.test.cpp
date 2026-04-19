#include "dansandu/radiance/radiance.hpp"

#include <string>

namespace
{

struct ToStringType
{
    std::string toString() const
    {
        return "ToStringType";
    }
};

bool operator==(ToStringType, ToStringType)
{
    return true;
}

struct ToWideStringType
{
    std::wstring toWideString() const
    {
        return L"ToWideStringType";
    }
};

bool operator==(ToWideStringType, ToWideStringType)
{
    return true;
}

}

TEST_CASE("to_string")
{
    REQUIRE(ToStringType() == ToStringType());

    REQUIRE(ToWideStringType() == ToWideStringType());
}
