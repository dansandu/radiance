#include "dansandu/journey/logging.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::radiance::exception::AssertionFailureException;

TEST_CLASS(CaughtSetUpAssertionFailureTest)
{
public:
    void setUp() override
    {
        try
        {
            REQUIRE(false);
        }
        catch (const AssertionFailureException&)
        {
        }
    }

    void cleanUp() override
    {
        LOG_ERROR("cleanUp ran after a failed setUp");
    }

    TEST_METHOD(method_after_caught_set_up_assertion_failure)
    {
        LOG_ERROR("test method ran after a failed setUp");
    }
};
