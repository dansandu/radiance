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

    const auto vectorMap = std::map<std::string, std::vector<int>>{{
        {"even", {0, 2, 4, 6}},
        {"odd", {1, 3, 5, 7}},
    }};

    REQUIRE(vectorMap == vectorMap);
}
