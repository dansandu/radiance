#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

TEST_CLASS(CleanUpFailureTest)
{
public:
    void cleanUp() override
    {
        throw std::runtime_error{"cleanup failed"};
    }

    TEST_METHOD(failing_cleanup_with_successful_method)
    {
    }

    TEST_METHOD(failing_cleanup_with_failing_method)
    {
        throw std::domain_error{"method failed"};
    }
};
