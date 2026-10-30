

#include "dansandu/journey/logging.hpp"
#include "dansandu/radiance/base_test_class.hpp"

class MyTestClass : private ::dansandu::radiance::base_test_class::BaseTestClass<MyTestClass>
{
public:
    MyTestClass()
    {
        LOG_INFO("constructor MyTestClass");
    }

    ~MyTestClass() noexcept
    {
        LOG_INFO("destructor MyTestClass");
    }
};
