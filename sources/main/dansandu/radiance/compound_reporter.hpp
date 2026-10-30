#pragma once

#include "dansandu/radiance/reporter.hpp"

#include <functional>
#include <vector>

namespace dansandu::radiance::compound_reporter
{

class PRALINE_EXPORT CompoundReporter : public dansandu::radiance::reporter::IReporter
{
public:
    explicit CompoundReporter(std::vector<std::reference_wrapper<dansandu::radiance::reporter::IReporter>> reporters);

    void testSuiteBegin(const TestSuiteMetadata& testSuiteMetadata) override;

    void testSuiteEnd(const TestSuiteResult& testSuiteResult) override;

    void testClassBegin(const TestClassMetadata& testClassMetadata) override;

    void testClassEnd(const TestClassResult& testClassResult) override;

    void testMethodBegin(const TestMethodMetadata& testMethodMetadata) override;

    void testMethodEnd(const TestMethodResult& testMethodResult) override;

    void assertionBegin(const AssertionMetadata& assertionMetadata) override;

    void assertionEnd(const AssertionResult& assertionResult) override;

private:
    std::vector<std::reference_wrapper<dansandu::radiance::reporter::IReporter>> reporters_;
};

}
