#pragma once

#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/base_test_class.hpp"

#include <string>
#include <vector>

namespace dansandu::radiance::test_class_descriptor
{

using TestClassProducer = std::unique_ptr<dansandu::radiance::base_test_class::BaseTestClass>(*)();

using TestMethodInvoker = void(*)(void*);

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
        const std::string& testClassName,
        const std::string& testClassFilePath,
        const int testClassLineNumber,
        const TestClassProducer testClassProducer,
        std::vector<TestMethodDescriptor>&& testMethodDescriptors
    );

    TestClassResult runTestClass(
        const TestSuiteMetadata& testSuiteMetadata, dansandu::radiance::reporter::IReporter& reporter
    ) const;

private:
    std::string testClassName_;
    std::string testClassFilePath_;
    int testClassLineNumber_;
    TestClassProducer testClassProducer_;
    std::vector<TestMethodDescriptor> testMethodDescriptors_;
};

}
