#include "dansandu/radiance/radiance.hpp"

#include <array>

TEST_CASE("type_array")
{
    const auto myIntegers = std::array<int, 5>{{1, 2, 3, 4, 5}};

    REQUIRE(myIntegers == myIntegers);
}
