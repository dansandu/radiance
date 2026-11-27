#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::radiance::exception::AssertionFailureException;

TEST_CLASS(CaughtCleanUpAssertionFailureTest)
{
public:
    void cleanUp() override
    {
        try
        {
            REQUIRE(false);
        }
        catch (const AssertionFailureException&)
        {
        }
    }

    TEST_METHOD(method_before_caught_clean_up_assertion_failure)
    {
    }
};
