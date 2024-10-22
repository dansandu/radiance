#include "dansandu/radiance/radiance.hpp"

#include <vector>

TEST_CASE("type_vector")
{
    const auto myIntegers = std::vector<int>{{1, 2, 3, 4}};

    REQUIRE(myIntegers == myIntegers);

    const auto myStrings = std::vector<const char*>{{"a", "b", "c"}};

    REQUIRE(myStrings == myStrings);

    const auto myBinary = std::vector<unsigned char>{{0x01, 0x0A, 0xC1, 0xFF}};

    REQUIRE(myBinary == myBinary);
}
