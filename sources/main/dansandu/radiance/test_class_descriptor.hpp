#pragma once

#include "dansandu/radiance/base_test_class.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace dansandu::radiance::test_class_descriptor
{

using TestClassProducer = std::unique_ptr<dansandu::radiance::base_test_class::BaseTestClass> (*)();

// Test methods are discovered as member functions of the subclass, so they must be invoked with the address of the
// subclass object. The address of the base class subobject can differ from it (e.g. with multiple inheritance).
using SubclassAddressResolver = void* (*)(dansandu::radiance::base_test_class::BaseTestClass & testClass);

using TestMethodInvoker = std::function<void(void* subclassAddress)>;

struct TestMethodDescriptor
{
    std::string testMethodName;
    int testMethodLineNumber;
    TestMethodInvoker testMethodInvoker;
};

class TestClassDescriptor
{
public:
    TestClassDescriptor(
        const std::string& testClassName, const std::string& testClassFilePath, const int testClassLineNumber,
        const TestClassProducer testClassProducer, const SubclassAddressResolver subclassAddressResolver,
        const std::vector<TestMethodDescriptor>& testMethodDescriptors
    );

    TestClassResult
    runTestClass(const TestSuiteMetadata& testSuiteMetadata, dansandu::radiance::reporter::IReporter& reporter) const;

private:
    std::string testClassName_;
    std::string testClassFilePath_;
    int testClassLineNumber_;
    TestClassProducer testClassProducer_;
    SubclassAddressResolver subclassAddressResolver_;
    std::vector<TestMethodDescriptor> testMethodDescriptors_;
};

}
