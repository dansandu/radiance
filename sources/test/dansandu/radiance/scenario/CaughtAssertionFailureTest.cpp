#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::radiance::exception::AssertionFailureException;

TEST_CLASS(CaughtAssertionFailureTest)
{
public:
    TEST_METHOD(caught_failing_assertion)
    {
        try
        {
            REQUIRE(false);
        }
        catch (const AssertionFailureException&)
        {
        }
    }
};
