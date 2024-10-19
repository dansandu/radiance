#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

namespace
{

struct MyType
{
};

static bool operator==(MyType, MyType)
{
    throw std::runtime_error{"expression"};
}

}

TEST_CASE(L"exception_expression")
{
    REQUIRE(MyType{} == MyType{});
}
