#include "dansandu/journey/logging.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

TEST_CLASS(ConstructionFailureTest)
{
public:
    ConstructionFailureTest()
    {
        throw std::runtime_error{"construction failed"};
    }

    void setUp() override
    {
        LOG_ERROR("setUp ran after a failed construction");
    }

    TEST_METHOD(method_after_failing_construction)
    {
        LOG_ERROR("test method ran after a failed construction");
    }
};
