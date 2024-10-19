#include "dansandu/radiance/radiance.hpp"

int f()
{
    return 5;
}

TEST_CASE(L"type_int")
{
    REQUIRE(23 % 10 == 3);

    REQUIRE(f());
}
