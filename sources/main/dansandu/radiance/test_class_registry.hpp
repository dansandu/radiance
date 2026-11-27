#pragma once

#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/test_class_descriptor.hpp"

#include <map>
#include <mutex>
#include <vector>

namespace dansandu::radiance::test_class_registry
{

struct TestClassRegistryEntry
{
    std::string testClassName;
    std::string testClassFilePath;
    int testClassLineNumber;
    dansandu::radiance::test_class_descriptor::TestClassProducer testClassProducer;
    dansandu::radiance::test_class_descriptor::SubclassAddressResolver subclassAddressResolver;
};

class PRALINE_EXPORT TestClassRegistry
{
public:
    static TestClassRegistry& getGlobalInstance();

    bool registerTestClass(TestClassRegistryEntry&& testClassRegistryEntry);

    std::map<std::string, dansandu::radiance::test_class_descriptor::TestClassDescriptor>
    getTestClassDescriptors(const std::string& libraryName);

private:
    std::vector<TestClassRegistryEntry> testClassRegistry_;
    std::mutex mutex_;
};

}
