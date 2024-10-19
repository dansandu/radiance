#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

namespace
{

struct MyType
{
};

static MyType throws(const char* const message)
{
    throw std::runtime_error{message};
}

static bool operator==(MyType, MyType)
{
    return true;
}

}

TEST_CASE(L"exception_first_argument")
{
    REQUIRE(throws("first") == MyType{});
}
