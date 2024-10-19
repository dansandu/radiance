#include "dansandu/radiance/reporter.hpp"

namespace dansandu::radiance::reporter
{

IReporter::~IReporter() noexcept
{
}

void IReporter::testSuiteBegin(const TestSuiteMetadata&)
{
}

void IReporter::testSuiteEnd(const TestSuiteResult&)
{
}

void IReporter::testCaseBegin(const TestCaseMetadata&)
{
}

void IReporter::testCaseEnd(const TestCaseResult&)
{
}

void IReporter::testCaseRunBegin(const TestCaseRunMetadata&)
{
}

void IReporter::testCaseRunEnd(const TestCaseRunResult&)
{
}

void IReporter::sectionBegin(const SectionMetadata&)
{
}

void IReporter::sectionEnd(const SectionResult&)
{
}

void IReporter::assertionBegin(const AssertionMetadata&)
{
}

void IReporter::assertionEnd(const AssertionResult&)
{
}

}
