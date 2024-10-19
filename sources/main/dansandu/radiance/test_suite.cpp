#include "dansandu/radiance/test_suite.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/section_scheduler.hpp"

#include <type_traits>
#include <typeinfo>

using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::section_scheduler::SectionScheduler;
using dansandu::radiance::test_case::TestCase;

namespace dansandu::radiance::test_suite
{

TestSuite::TestSuite(const TestSuiteMetadata& testSuiteMetadata,
                     const std::vector<TestCase::Descriptor>& testCaseDescriptors, IReporter& reporter)
    : testSuiteResult_{.testSuiteMetadata = testSuiteMetadata,
                       .testCasesRan = 0,
                       .testCasesPassed = 0,
                       .testCasesFailed = 0,
                       .assertionsRan = 0,
                       .assertionsPassed = 0,
                       .assertionsFailed = 0,
                       .testSuiteSuccess = true},
      testCaseDescriptors_{testCaseDescriptors},
      reporter_{reporter}
{
    if (testSuiteResult_.testSuiteMetadata.testCasesBeingRun != std::ssize(testCaseDescriptors))
    {
        throw std::logic_error{"metadata number of tests being run must match the number of test descriptors"};
    }

    if (testSuiteResult_.testSuiteMetadata.testCasesTotal < std::ssize(testCaseDescriptors))
    {
        throw std::logic_error{
            "metadata total number of tests must be greater of equal to the number of test descriptors"};
    }

    reporter_.testSuiteBegin(testSuiteResult_.testSuiteMetadata);
}

TestSuite::~TestSuite() noexcept
{
    reporter_.testSuiteEnd(testSuiteResult_);
}

void TestSuite::run()
{
    for (const auto& descriptor : testCaseDescriptors_)
    {
        ++testSuiteResult_.testCasesRan;

        const auto testCaseMetadata = TestCaseMetadata{
            .testSuiteMetadata = testSuiteResult_.testSuiteMetadata,
            .testCaseName = descriptor.testCaseName,
            .filePath = descriptor.filePath,
            .lineNumber = descriptor.lineNumber,
        };

        auto testCase = TestCase{testCaseMetadata, descriptor, reporter_};

        testCase.run();

        if (testCase.testCaseResult().testCaseSuccess)
        {
            ++testSuiteResult_.testCasesPassed;
        }
        else
        {
            ++testSuiteResult_.testCasesFailed;
        }

        testSuiteResult_.assertionsRan += testCase.testCaseResult().assertionsRan;
        testSuiteResult_.assertionsPassed += testCase.testCaseResult().assertionsPassed;
        testSuiteResult_.assertionsFailed += testCase.testCaseResult().assertionsFailed;

        testSuiteResult_.testSuiteSuccess =
            testSuiteResult_.testSuiteSuccess && testCase.testCaseResult().testCaseSuccess;
    }
}

const TestSuiteResult& TestSuite::testSuiteResult() const
{
    return testSuiteResult_;
}

}
