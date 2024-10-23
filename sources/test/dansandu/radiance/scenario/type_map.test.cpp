#include "dansandu/radiance/radiance.hpp"

#include <map>

TEST_CASE("type_map")
{
    const auto someIntegersMap = std::map<std::string, int>{{
        {"key1", 123},
        {"key2", 456},
        {"key3", 789},
    }};

    const auto anotherIntegersMap = std::map<std::string, int>{{
        {"key2", 123},
        {"key3", 0},
    }};

    REQUIRE(someIntegersMap == anotherIntegersMap);
}
