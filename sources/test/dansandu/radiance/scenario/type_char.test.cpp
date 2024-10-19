#include "dansandu/radiance/radiance.hpp"

TEST_CASE(L"type_char")
{
    REQUIRE('\t' != '\n');

    const auto character = L'\'';

    REQUIRE(character == L'\'');
}
