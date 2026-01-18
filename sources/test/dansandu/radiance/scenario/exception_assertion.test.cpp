#include "dansandu/journey/exception.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

using dansandu::journey::exception::WideException;

TEST_CASE("exception_assertion")
{
    SECTION("WideException throw match")
    {
        const auto myFunction = []() { throw WideException{L"WideException message"}; };

        REQUIRE_THROW(WideException, myFunction());
    }

    SECTION("WideException throw mismatch")
    {
        const auto myFunction = []() { throw WideException{L"WideException message"}; };

        REQUIRE_THROW(std::exception, myFunction());
    }

    SECTION("std::logic_error throw match")
    {
        const auto myFunction = []() { throw std::logic_error{"std::logic_error message"}; };

        REQUIRE_THROW(std::logic_error, myFunction());
    }

    SECTION("std::logic_error throw mismatch")
    {
        const auto myFunction = []() { throw std::logic_error{"std::logic_error message"}; };

        REQUIRE_THROW(std::runtime_error, myFunction());
    }

    SECTION("no throw mismatch")
    {
        const auto myFunction = []() {};

        REQUIRE_THROW(std::exception, myFunction());
    }

    SECTION("int throw match")
    {
        const auto myFunction = []() { throw int(7); };

        REQUIRE_THROW(int, myFunction());
    }

    SECTION("int throw mismatch")
    {
        const auto myFunction = []() { throw int(13); };

        REQUIRE_THROW(std::exception, myFunction());
    }
}
