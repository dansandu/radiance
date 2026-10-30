#include "dansandu/radiance/test_suite.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/journey/common.hpp"

using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::test_class_descriptor::TestClassDescriptor;
using dansandu::journey::Level;

namespace dansandu::radiance::test_suite
{

TestSuiteResult runTestSuite(
    const std::vector<TestClassDescriptor>& testClassDescriptors, const int testClassesTotal,
    const Level loggingLevelFailure, IReporter& reporter
)
{
    const auto testClassesToRun = static_cast<int>(testClassDescriptors.size());

    if (testClassesTotal < testClassesToRun)
    {
        THROW(std::logic_error, "Total number of test classes ", testClassesTotal, " must be greater of equal to the number of test classes to run ", testClassesToRun);
    }

    auto testSuiteResult = TestSuiteResult{
        .testSuiteMetadata = TestSuiteMetadata{
            .testClassesTotal = testClassesTotal,
            .testClassesToRun = testClassesToRun,
            .testClassesToSkip = testClassesTotal - testClassesToRun,
            .loggingLevelFailure = loggingLevelFailure,
        },
        .testClassesRan = 0,
        .testClassesPassed = 0,
        .testClassesFailed = 0,
        .assertionsRan = 0,
        .assertionsPassed = 0,
        .assertionsFailed = 0,
        .loggingSuccess = true,
        .testSuiteSuccess = true,
    };

    reporter.testSuiteBegin(testSuiteResult.testSuiteMetadata);

    for (const auto& testClassDescriptor : testClassDescriptors)
    {
        ++testSuiteResult.testClassesRan;

        const auto testClassResult = testClassDescriptor.runTestClass(testSuiteResult.testSuiteMetadata, reporter);

        if (testClassResult.testClassSuccess)
        {
            ++testSuiteResult.testClassesPassed;
        }
        else
        {
            ++testSuiteResult.testClassesFailed;
        }

        testSuiteResult.assertionsRan += testClassResult.assertionsRan;
        testSuiteResult.assertionsPassed += testClassResult.assertionsPassed;
        testSuiteResult.assertionsFailed += testClassResult.assertionsFailed;
        testSuiteResult.loggingSuccess &= testClassResult.loggingSuccess;
        testSuiteResult.testSuiteSuccess &= testClassResult.testClassSuccess;
    }

    reporter.testSuiteEnd(testSuiteResult);

    return testSuiteResult;
}

}
