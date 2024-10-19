#include "dansandu/radiance/radiance.hpp"

TEST_CASE(L"type_string")
{
    REQUIRE(std::string{"\x1B[32mGreen\x1B[0m\n\"text\" \\\t"} != "string");

    const auto myWideString = std::wstring{L"Wide string"};

    REQUIRE(myWideString == myWideString);

    REQUIRE(!myWideString.empty());

    REQUIRE(myWideString.size());

    REQUIRE(myWideString.size() == 11U);
}
