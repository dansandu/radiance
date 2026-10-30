#pragma once

#include "dansandu/radiance/common.hpp"

#include <memory>
#include <mutex>
#include <vector>

namespace dansandu::radiance::test_class_registry
{

class PRALINE_EXPORT TestClassRegistry
{
public:
    static TestClassRegistry& getGlobalInstance();

    bool registerTestClass(TestClassDescriptor&& testClassDescriptor);

    std::vector<TestClassDescriptor> getTestClassDescriptors();

private:
    std::vector<TestClassDescriptor> testClassDescriptors_;
    std::mutex mutex_;
};

}
