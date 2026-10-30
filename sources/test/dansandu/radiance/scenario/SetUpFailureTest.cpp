#include "dansandu/journey/logging.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

TEST_CLASS(SetUpFailureTest)
{
public:
    void setUp() override
    {
        throw std::runtime_error{"setup failed"};
    }

    void cleanUp() override
    {
        LOG_ERROR("cleanUp ran after a failed setUp");
    }

    TEST_METHOD(method_after_failing_set_up)
    {
        LOG_ERROR("test method ran after a failed setUp");
    }
};
