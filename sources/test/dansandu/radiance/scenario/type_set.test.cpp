#include "dansandu/radiance/radiance.hpp"

#include <set>

TEST_CASE("type_set")
{
    const auto someIntegersSet = std::set<int>{{1, 2, 3, 4}};

    const auto anotherIntegersMap = std::set<int>{{3, 4, 5}};

    REQUIRE(someIntegersSet == anotherIntegersMap);
}
