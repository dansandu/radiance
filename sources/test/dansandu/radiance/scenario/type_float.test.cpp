#include "dansandu/radiance/radiance.hpp"

TEST_CASE("type_float")
{
    REQUIRE(1000.12345678f > 0.0f);

    REQUIRE(99999.123456789012345 > 11.0);

    REQUIRE(-1.12345678f > -2.98765f);

    REQUIRE(-1.123456789012345 > -2.987654321);
}
