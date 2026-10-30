#pragma once

#include "dansandu/radiance/reporter.hpp"

#include <sstream>

namespace dansandu::radiance::test_reporter
{

class TestReporter : public dansandu::radiance::reporter::IReporter
{
public:
    TestReporter();

    void testSuiteBegin(const TestSuiteMetadata& metadata) override;

    void testSuiteEnd(const TestSuiteResult& result) override;

    void testClassBegin(const TestClassMetadata& metadata) override;

    void testClassEnd(const TestClassResult& result) override;

    void testMethodBegin(const TestMethodMetadata& metadata) override;

    void testMethodEnd(const TestMethodResult& result) override;

    void assertionBegin(const AssertionMetadata& metadata) override;

    void assertionEnd(const AssertionResult& result) override;

    std::string getString() const;

private:
    std::ostringstream stream_;
};

}
