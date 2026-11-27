#include "dansandu/radiance/radiance.hpp"

TEST_CLASS(RequireStatementTest)
{
public:
    TEST_METHOD(require_in_if_statement_without_braces)
    {
        const auto value = 7;

        if (value % 2 == 1)
            REQUIRE(value + 1 == 8);
        else
            REQUIRE(value == 8);
    }
};
