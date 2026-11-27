#include "dansandu/journey/logging.hpp"
#include "dansandu/radiance/radiance.hpp"

TEST_CLASS(AssertionFailureWithCleanUpTest)
{
public:
    AssertionFailureWithCleanUpTest() : cleanedUp_{false}
    {
    }

    ~AssertionFailureWithCleanUpTest() noexcept
    {
        if (!cleanedUp_)
        {
            LOG_ERROR("cleanUp did not run");
        }
    }

    void cleanUp() override
    {
        cleanedUp_ = true;
    }

    TEST_METHOD(failing_assertion)
    {
        REQUIRE(false);
    }

private:
    bool cleanedUp_;
};
