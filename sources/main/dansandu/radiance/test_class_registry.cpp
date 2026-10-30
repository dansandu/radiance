#include "dansandu/radiance/test_class_registry.hpp"

namespace dansandu::radiance::test_class_registry
{

TestClassRegistry& TestClassRegistry::getGlobalInstance()
{
    static auto globalTestClassRegistry = TestClassRegistry{};

    return globalTestClassRegistry;
}

bool TestClassRegistry::registerTestClass(TestClassDescriptor&& testClassDescriptor)
{
    const auto lock = std::lock_guard{mutex_};

    testClassDescriptors_.push_back(std::move(testClassDescriptor));

    return true;
}

std::vector<TestClassDescriptor> TestClassRegistry::getTestClassDescriptors()
{
    const auto lock = std::lock_guard{mutex_};

    return testClassDescriptors_;
}

}
