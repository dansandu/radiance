#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

namespace
{

struct MyType
{
};

MyType throws(const char* const message)
{
    throw std::runtime_error{message};
}

bool operator==(MyType, MyType)
{
    return true;
}

}

TEST_CASE(L"exception_second_argument")
{
    REQUIRE(MyType{} == throws("second"));
}
