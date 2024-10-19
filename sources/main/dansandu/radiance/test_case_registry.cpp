
#include "dansandu/radiance/test_case_registry.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/test_case.hpp"
#include "dansandu/radiance/test_suite.hpp"

#include <algorithm>

using dansandu::radiance::exception::DuplicateTestCaseNameException;
using dansandu::radiance::exception::TestCaseWithNameNotFoundException;
using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::test_case::TestCase;
using dansandu::radiance::test_suite::TestSuite;

namespace dansandu::radiance::test_case_registry
{

template<typename Container, typename Predicate>
bool containsIf(const Container& container, Predicate&& predicate)
{
    return std::find_if(container.cbegin(), container.cend(), std::forward<Predicate>(predicate)) != container.cend();
}

TestCaseRegistry& TestCaseRegistry::instance()
{
    static auto testCaseRegistry = TestCaseRegistry{};
    return testCaseRegistry;
}

bool TestCaseRegistry::registerTestCase(TestCase::Descriptor descriptor)
{
    if (containsIf(testCaseDescriptors_, [&](const auto& d) { return d.testCaseName == descriptor.testCaseName; }))
    {
        throw DuplicateTestCaseNameException{descriptor.testCaseName};
    }

    testCaseDescriptors_.push_back(std::move(descriptor));

    return true;
}

TestSuiteResult TestCaseRegistry::runTestCase(const std::wstring& testCaseName, IReporter& reporter) const
{
    const auto testCasePosition = std::find_if(testCaseDescriptors_.cbegin(), testCaseDescriptors_.cend(),
                                               [&](const auto& d) { return d.testCaseName == testCaseName; });
    if (testCasePosition == testCaseDescriptors_.cend())
    {
        throw TestCaseWithNameNotFoundException{testCaseName};
    }

    const auto testCasesToRun = std::vector<TestCase::Descriptor>{*testCasePosition};

    const auto testCasesTotal = static_cast<int>(testCaseDescriptors_.size());

    const auto testCasesBeingRun = static_cast<int>(testCasesToRun.size());

    const auto testSuiteMetadata = TestSuiteMetadata{
        .testCasesTotal = testCasesTotal,
        .testCasesBeingRun = testCasesBeingRun,
        .testCasesBeingSkipped = testCasesTotal - testCasesBeingRun,
    };

    auto testSuite = TestSuite{testSuiteMetadata, testCasesToRun, reporter};
    testSuite.run();

    return testSuite.testSuiteResult();
}

TestSuiteResult TestCaseRegistry::runAllTestCases(IReporter& reporter) const
{
    auto testCasesToRun = testCaseDescriptors_;
    std::sort(testCasesToRun.begin(), testCasesToRun.end(),
              [](const auto& a, const auto& b) { return a.testCaseName < b.testCaseName; });

    const auto testCasesTotal = static_cast<int>(testCaseDescriptors_.size());

    const auto testCasesBeingRun = static_cast<int>(testCasesToRun.size());

    const auto testSuiteMetadata = TestSuiteMetadata{
        .testCasesTotal = testCasesTotal,
        .testCasesBeingRun = testCasesBeingRun,
        .testCasesBeingSkipped = testCasesTotal - testCasesBeingRun,
    };

    auto testSuite = TestSuite{testSuiteMetadata, testCasesToRun, reporter};
    testSuite.run();

    return testSuite.testSuiteResult();
}

}
