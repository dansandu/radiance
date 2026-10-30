#include "dansandu/radiance/radiance.hpp"

TEST_CASE("type_string")
{
    REQUIRE(std::string{"\x1B[32mGreen\x1B[0m\n\"text\" \\\t"} != "string");

    const auto myWideString = std::string{"Wide string"};

    REQUIRE(myWideString == myWideString);

    REQUIRE(!myWideString.empty());
}
