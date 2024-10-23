#include "dansandu/radiance/radiance.hpp"

TEST_CASE("exception_multiple_sections")
{
    SECTION("1")
    {
        SECTION("11")
        {
            REQUIRE(11);

            SECTION("111")
            {
                REQUIRE(111);
            }

            throw std::runtime_error{"error section 11"};
        }

        SECTION("12")
        {
            throw std::runtime_error{"error section 12"};
        }

        SECTION("13")
        {
            REQUIRE(13);
        }
    }

    SECTION("2")
    {
        throw std::runtime_error{"error section 2"};
    }

    REQUIRE(0 == 0);
}
