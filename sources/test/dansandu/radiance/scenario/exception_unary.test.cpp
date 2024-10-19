#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

namespace
{

struct MyType
{
    bool throws(const char* const message)
    {
        throw std::runtime_error{message};
    }
};

}

TEST_CASE(L"exception_unary")
{
    REQUIRE(MyType().throws("unary"));
}
