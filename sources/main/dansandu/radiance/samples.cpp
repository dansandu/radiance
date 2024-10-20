#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

TEST_CASE(L"sample1")
{
    const auto myInteger = 12;

    REQUIRE(myInteger > 15);
}

TEST_CASE(L"sample2")
{
    const auto myBoolean = false;

    SECTION(L"outer")
    {
        SECTION(L"inner")
        {
            REQUIRE(myBoolean);
        }
    }
}

TEST_CASE(L"sample3")
{
    REQUIRE(15 / 4 == 3);
}

TEST_CASE(L"sample4")
{
    const auto f = []() -> int { throw std::runtime_error("my error"); };

    REQUIRE(f() == 13);
}

TEST_CASE(L"sample5")
{
    const auto f = []() -> int { throw std::runtime_error("my error"); };

    f();

    REQUIRE(f() == 13);
}
