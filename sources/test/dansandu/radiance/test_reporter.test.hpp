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

    void testCaseBegin(const TestCaseMetadata& metadata) override;

    void testCaseEnd(const TestCaseResult& result) override;

    void testCaseRunBegin(const TestCaseRunMetadata& metadata) override;

    void testCaseRunEnd(const TestCaseRunResult& result) override;

    void sectionBegin(const SectionMetadata& metadata) override;

    void sectionEnd(const SectionResult& result) override;

    void assertionBegin(const AssertionMetadata& metadata) override;

    void assertionEnd(const AssertionResult& result) override;

    std::wstring getString() const;

private:
    std::wostringstream stream_;
};

}
