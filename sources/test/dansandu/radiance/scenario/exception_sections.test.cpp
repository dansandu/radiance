#include "dansandu/radiance/radiance.hpp"

TEST_CASE("exception_sections")
{
    SECTION("1")
    {
        SECTION("11")
        {
            SECTION("111")
            {
                REQUIRE(12 == 12);

                throw std::runtime_error{"error section 111"};
            }

            SECTION("112")
            {
            }

            REQUIRE(1 == 1);

            SECTION("113")
            {
                REQUIRE('a' == 'a');

                REQUIRE('a' == 'b');
            }

            REQUIRE(2 == 2);
        }

        SECTION("12")
        {
        }
    }

    SECTION("2")
    {
        throw std::runtime_error{"error section 2"};
    }
}
