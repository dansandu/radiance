#include "dansandu/radiance/radiance.hpp"

#include <string>

namespace
{

struct ToStringMethod
{
    std::string toString() const
    {
        return "ToStringMethod()";
    }
};

bool operator==(ToStringMethod, ToStringMethod)
{
    return true;
}

struct ToStringFunction
{
};

bool operator==(ToStringFunction, ToStringFunction)
{
    return true;
}

std::string toString(ToStringFunction)
{
    return "ToStringFunction()";
}

}

TEST_CASE("to_string")
{
    REQUIRE(ToStringMethod() == ToStringMethod());

    REQUIRE(ToStringFunction() == ToStringFunction());
}
