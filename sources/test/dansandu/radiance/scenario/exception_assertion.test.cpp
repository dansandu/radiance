#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

TEST_CASE("exception_assertion")
{
    SECTION("good std::exception")
    {
        const auto myFunction = []() { throw std::exception{"my error"}; };

        REQUIRE_THROW(myFunction(), std::exception);
    }

    SECTION("good std::runtime_error")
    {
        const auto myFunction = []() { throw std::runtime_error{"my error"}; };

        REQUIRE_THROW(myFunction(), std::runtime_error);
    }

    SECTION("bad std::logic_error")
    {
        const auto myFunction = []() { throw std::logic_error{"my error"}; };

        REQUIRE_THROW(myFunction(), std::runtime_error);
    }

    SECTION("bad no throw")
    {
        const auto myFunction = []() {};

        REQUIRE_THROW(myFunction(), std::runtime_error);
    }
}
