#include "dansandu/journey/logging.hpp"
#include "dansandu/radiance/radiance.hpp"

TEST_CLASS(LoggingFailureTest)
{
public:
    TEST_METHOD(logs_below_limit)
    {
        LOG_WARNING("first warning");

        LOG_ERROR("first error");
    }

    TEST_METHOD(logs_above_limit)
    {
        for (auto index = 1; index <= 6; ++index)
        {
            LOG_ERROR("error number ", index);
        }
    }
};
