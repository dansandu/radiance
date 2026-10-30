#include "dansandu/journey/exception.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

using dansandu::journey::exception::Exception;

TEST_CLASS(RequireThrowTest)
{
public:
    TEST_METHOD(exception_throw_match)
    {
        REQUIRE_THROW(Exception, throw Exception{"Exception match message"});
    }

    TEST_METHOD(exception_throw_mismatch)
    {
        REQUIRE_THROW(std::exception, throw Exception{"Exception mismatch message"});
    }

    TEST_METHOD(logic_error_throw_match)
    {
        REQUIRE_THROW(std::logic_error, throw std::logic_error{"std::logic_error match message"});
    }

    TEST_METHOD(logic_error_throw_mismatch)
    {
        REQUIRE_THROW(std::runtime_error, throw std::logic_error{"std::logic_error mismatch message"});
    }

    TEST_METHOD(no_throw_mismatch)
    {
        REQUIRE_THROW(std::exception, ;);
    }
};
