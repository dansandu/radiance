
#include "dansandu/radiance/test_case_registry.hpp"
#include "dansandu/journey/common.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/progress_bar_console_reporter.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/test_case.hpp"
#include "dansandu/radiance/test_suite.hpp"
#include "dansandu/radiance/utility.hpp"

#include <algorithm>

using dansandu::journey::Level;
using dansandu::journey::utility::toWideString;
using dansandu::radiance::exception::DuplicateTestCaseNameException;
using dansandu::radiance::exception::TestCaseWithNameNotFoundException;
using dansandu::radiance::progress_bar_console_reporter::ProgressBarConsoleReporter;
using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::test_case::TestCase;
using dansandu::radiance::test_suite::TestSuite;
using dansandu::radiance::utility::getEnvironmentVariable;

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
        throw DuplicateTestCaseNameException{L"Duplicate test case name '" + descriptor.testCaseName + L"'"};
    }

    testCaseDescriptors_.push_back(std::move(descriptor));

    return true;
}

TestSuiteResult TestCaseRegistry::runTestCases(const std::vector<std::wstring>& testCasesNames,
                                               IReporter& reporter) const
{
    auto testCasesToRun = std::vector<TestCase::Descriptor>{};

    for (const auto& testCaseName : testCasesNames)
    {
        const auto testCasePosition = std::find_if(testCaseDescriptors_.cbegin(), testCaseDescriptors_.cend(),
                                                   [&](const auto& d) { return d.testCaseName == testCaseName; });
        if (testCasePosition == testCaseDescriptors_.cend())
        {
            throw TestCaseWithNameNotFoundException{L"No test case named '" + testCaseName + L"' was found"};
        }

        if (std::find_if(testCasesToRun.cbegin(), testCasesToRun.cend(),
                         [&](const auto& d) { return d.testCaseName == testCaseName; }) == testCasesToRun.cend())
        {
            testCasesToRun.push_back(*testCasePosition);
        }
    }

    const auto testCasesTotal = static_cast<int>(testCaseDescriptors_.size());

    const auto testCasesBeingRun = static_cast<int>(testCasesToRun.size());

    const auto testSuiteMetadata = TestSuiteMetadata{
        .testCasesTotal = testCasesTotal,
        .testCasesBeingRun = testCasesBeingRun,
        .testCasesBeingSkipped = testCasesTotal - testCasesBeingRun,
        .loggingLevelFailure = Level::warning,
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
        .loggingLevelFailure = Level::warning,
    };

    auto testSuite = TestSuite{testSuiteMetadata, testCasesToRun, reporter};
    testSuite.run();

    return testSuite.testSuiteResult();
}

int runTestSuite(const int argumentCount, const char* const* const arguments)
{
    auto reporter = ProgressBarConsoleReporter{};

    if (argumentCount > 0)
    {
        auto testCasesNames = std::vector<std::wstring>{};

        for (auto index = 0; index < argumentCount; ++index)
        {
            testCasesNames.push_back(toWideString(arguments[index]));
        }

        const auto testSuiteResult = TestCaseRegistry::instance().runTestCases(testCasesNames, reporter);

        return !testSuiteResult.testSuiteSuccess;
    }
    else
    {
        const auto testSuiteResult = TestCaseRegistry::instance().runAllTestCases(reporter);

        return !testSuiteResult.testSuiteSuccess;
    }
}

}
