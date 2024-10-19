#pragma once

#include "dansandu/radiance/common.hpp"

namespace dansandu::radiance::reporter
{

class PRALINE_EXPORT IReporter
{
public:
    virtual ~IReporter() noexcept = 0;

    virtual void testSuiteBegin(const TestSuiteMetadata& metadata);

    virtual void testSuiteEnd(const TestSuiteResult& result);

    virtual void testCaseBegin(const TestCaseMetadata& metadata);

    virtual void testCaseEnd(const TestCaseResult& result);

    virtual void testCaseRunBegin(const TestCaseRunMetadata& metadata);

    virtual void testCaseRunEnd(const TestCaseRunResult& result);

    virtual void sectionBegin(const SectionMetadata& metadata);

    virtual void sectionEnd(const SectionResult& result);

    virtual void assertionBegin(const AssertionMetadata& metadata);

    virtual void assertionEnd(const AssertionResult& result);
};

}
