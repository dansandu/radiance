#include "dansandu/radiance/test_class_registry.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/test_class_descriptor.hpp"

#include <map>
#include <mutex>
#include <vector>

namespace dansandu::radiance::test_class_registry
{

TestClassRegistry& TestClassRegistry::getGlobalInstance()
{
    static auto globalTestClassRegistry = TestClassRegistry{};

    return globalTestClassRegistry;
}

bool TestClassRegistry::registerTestClass(TestClassRegistryEntry&& testClassRegistryEntry)
{
    const auto lock = std::lock_guard{mutex_};

    testClassRegistry_.push_back(std::move(testClassRegistryEntry));

    return true;
}

std::map<std::string, TestClassDescriptor> TestClassRegistry::getTestClassDescriptors()
{
    auto testClassRegistryCopy = std::vector<TestClassRegistryEntry>{};

    {
        const auto lock = std::lock_guard{mutex_};

        testClassRegistryCopy = testClassRegistry_;
    }

    
}

}
