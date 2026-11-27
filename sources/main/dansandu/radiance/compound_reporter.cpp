#include "dansandu/radiance/compound_reporter.hpp"

#include <utility>

using dansandu::radiance::reporter::IReporter;

namespace dansandu::radiance::compound_reporter
{

CompoundReporter::CompoundReporter(std::vector<std::reference_wrapper<IReporter>> reporters)
    : reporters_{std::move(reporters)}
{
}

void CompoundReporter::testSuiteBegin(const TestSuiteMetadata& testSuiteMetadata)
{
    for (auto& reporter : reporters_)
    {
        reporter.get().testSuiteBegin(testSuiteMetadata);
    }
}

void CompoundReporter::testSuiteEnd(const TestSuiteResult& testSuiteResult)
{
    for (auto& reporter : reporters_)
    {
        reporter.get().testSuiteEnd(testSuiteResult);
    }
}

void CompoundReporter::testClassBegin(const TestClassMetadata& testClassMetadata)
{
    for (auto& reporter : reporters_)
    {
        reporter.get().testClassBegin(testClassMetadata);
    }
}

void CompoundReporter::testClassEnd(const TestClassResult& testClassResult)
{
    for (auto& reporter : reporters_)
    {
        reporter.get().testClassEnd(testClassResult);
    }
}

void CompoundReporter::testMethodBegin(const TestMethodMetadata& testMethodMetadata)
{
    for (auto& reporter : reporters_)
    {
        reporter.get().testMethodBegin(testMethodMetadata);
    }
}

void CompoundReporter::testMethodEnd(const TestMethodResult& testMethodResult)
{
    for (auto& reporter : reporters_)
    {
        reporter.get().testMethodEnd(testMethodResult);
    }
}

void CompoundReporter::assertionBegin(const AssertionMetadata& assertionMetadata)
{
    for (auto& reporter : reporters_)
    {
        reporter.get().assertionBegin(assertionMetadata);
    }
}

void CompoundReporter::assertionEnd(const AssertionResult& assertionResult)
{
    for (auto& reporter : reporters_)
    {
        reporter.get().assertionEnd(assertionResult);
    }
}

}
