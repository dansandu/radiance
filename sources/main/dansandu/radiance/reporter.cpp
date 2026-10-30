#include "dansandu/radiance/reporter.hpp"

namespace dansandu::radiance::reporter
{

IReporter::IReporter()
{
}

IReporter::~IReporter() noexcept
{
}

void IReporter::testSuiteBegin(const TestSuiteMetadata&)
{
}

void IReporter::testSuiteEnd(const TestSuiteResult&)
{
}

void IReporter::testClassBegin(const TestClassMetadata&)
{
}

void IReporter::testClassEnd(const TestClassResult&)
{
}

void IReporter::testMethodBegin(const TestMethodMetadata&)
{
}

void IReporter::testMethodEnd(const TestMethodResult&)
{
}

void IReporter::assertionBegin(const AssertionMetadata&)
{
}

void IReporter::assertionEnd(const AssertionResult&)
{
}

}
