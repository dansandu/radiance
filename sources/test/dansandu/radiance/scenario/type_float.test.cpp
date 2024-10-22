#include "dansandu/radiance/radiance.hpp"

TEST_CASE("type_float")
{
    REQUIRE(0.12345678f > 0.0f);

    REQUIRE(0.123456789012345 > 0.0);
}
