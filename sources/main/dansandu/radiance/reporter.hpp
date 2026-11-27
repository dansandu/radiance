#pragma once

#include "dansandu/radiance/common.hpp"

namespace dansandu::radiance::reporter
{

class PRALINE_EXPORT IReporter
{
public:
    IReporter(const IReporter& other) = delete;
    IReporter(IReporter&& other) noexcept = delete;
    IReporter& operator=(const IReporter& other) = delete;
    IReporter& operator=(IReporter&& other) noexcept = delete;

    IReporter();

    virtual ~IReporter() noexcept = 0;

    virtual void testSuiteBegin(const TestSuiteMetadata& testSuiteMetadata);

    virtual void testSuiteEnd(const TestSuiteResult& testSuiteResult);

    virtual void testClassBegin(const TestClassMetadata& testClassMetadata);

    virtual void testClassEnd(const TestClassResult& testClassResult);

    virtual void testMethodBegin(const TestMethodMetadata& testMethodMetadata);

    virtual void testMethodEnd(const TestMethodResult& testMethodResult);

    virtual void assertionBegin(const AssertionMetadata& assertionMetadata);

    virtual void assertionEnd(const AssertionResult& assertionResult);
};

}
