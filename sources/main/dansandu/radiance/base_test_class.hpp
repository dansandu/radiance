#pragma once

#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/test_class_registry.hpp"

namespace dansandu::radiance::base_test_class
{

template<typename Subclass>
class BaseTestClass
{

public:
    BaseTestClass()
    {
        // Force registerTestClassResult_ instantiation.
        static_cast<void>(registerTestClassResult_);
    }

private:
    static const bool registerTestClassResult_;
};

template<typename Subclass>
const bool BaseTestClass<Subclass>::registerTestClassResult_ =
    ::dansandu::radiance::test_class_registry::TestClassRegistry::getGlobalInstance().registerTestClass(
        TestClassDescriptor{.testClassName = ::dansandu::journey::utility::getTypeName<Subclass>(true),
                            .testClassFactory = []()
                            {
                                return std::unique_ptr<void, void (*)(void*)>{
                                    new Subclass{}, [](void* object) { delete static_cast<Subclass*>(object); }};
                            }});

}
