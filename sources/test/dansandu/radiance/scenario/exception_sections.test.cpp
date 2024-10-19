#include "dansandu/radiance/radiance.hpp"

TEST_CASE(L"exception_sections")
{
    SECTION(L"1")
    {
        SECTION(L"11")
        {
            SECTION(L"111")
            {
                REQUIRE(12ULL == 12ULL);

                throw std::runtime_error{"error section 111"};
            }

            SECTION(L"112")
            {
            }

            REQUIRE(1 == 1);

            SECTION(L"113")
            {
                REQUIRE('a' == 'a');

                REQUIRE('a' == 'b');
            }

            REQUIRE(2 == 2);
        }

        SECTION(L"12")
        {
        }
    }

    SECTION(L"2")
    {
        throw std::runtime_error{"error section 2"};
    }
}
