#include "dansandu/radiance/radiance.hpp"

TEST_CASE(L"sections")
{
    SECTION(L"1")
    {
        SECTION(L"11")
        {
            SECTION(L"111")
            {
                REQUIRE(12ULL == 12ULL);
            }

            SECTION(L"112")
            {
            }

            REQUIRE(1 == 1);

            SECTION(L"113")
            {
                REQUIRE('a' == 'a');
            }

            REQUIRE(2 == 2);
        }

        SECTION(L"12")
        {
        }
    }

    SECTION(L"2")
    {
    }
}
