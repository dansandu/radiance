#include "dansandu/radiance/radiance.hpp"

TEST_CASE("single_section")
{
    REQUIRE(1);

    SECTION("1")
    {
        REQUIRE(2);
    }

    REQUIRE(3);
}
