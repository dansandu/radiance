#include "dansandu/journey/logging.hpp"
#include "dansandu/radiance/radiance.hpp"

TEST_CLASS(SetUpAndCleanUpSuccessTest)
{
public:
    SetUpAndCleanUpSuccessTest() : secret_{0}, name_{}
    {
    }

    ~SetUpAndCleanUpSuccessTest() noexcept
    {
        if (secret_ != -28 || name_ != "Sarah")
        {
            LOG_ERROR("cleanUp did not run");
        }
    }

    void setUp() override
    {
        secret_ = 17;

        name_ = "Jimmy";
    }

    void cleanUp() override
    {
        secret_ = -28;

        name_ = "Sarah";
    }

    TEST_METHOD(check_set_up)
    {
        REQUIRE(secret_ == 17);

        REQUIRE(name_ == "Jimmy");
    }

private:
    int secret_;
    std::string name_;
};
