#include "dansandu/radiance/test_suite.hpp"
#include "dansandu/journey/common.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/reporter.hpp"

using dansandu::journey::Level;
using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::test_class_descriptor::TestClassDescriptor;

namespace dansandu::radiance::test_suite
{

TestSuiteResult runTestSuite(
    const std::set<std::string>& testClassNames,
    const std::map<std::string, TestClassDescriptor>& testClassDescriptorsByClassName,
    const dansandu::journey::Level loggingLevelFailure, dansandu::radiance::reporter::IReporter& reporter
)
{
    const auto testClassesTotal = static_cast<int>(testClassDescriptorsByClassName.size());

    const auto testClassesToRun = testClassNames.empty() ? testClassesTotal : static_cast<int>(testClassNames.size());

    for (const auto& testClassName : testClassNames)
    {
        if (!testClassDescriptorsByClassName.contains(testClassName))
        {
            THROW(std::runtime_error, "Test class '", testClassName, "' was not found");
        }
    }

    auto testSuiteResult = TestSuiteResult{
        .testSuiteMetadata =
            TestSuiteMetadata{
                .testClassesTotal = testClassesTotal,
                .testClassesToRun = testClassesToRun,
                .testClassesToSkip = testClassesTotal - testClassesToRun,
                .loggingLevelFailure = loggingLevelFailure,
            },
        .testClassesRan = 0,
        .testClassesPassed = 0,
        .testClassesFailed = 0,
        .testMethodsRan = 0,
        .testMethodsPassed = 0,
        .testMethodsFailed = 0,
        .assertionsRan = 0,
        .assertionsPassed = 0,
        .assertionsFailed = 0,
        .loggingSuccess = true,
        .testSuiteSuccess = true,
    };

    reporter.testSuiteBegin(testSuiteResult.testSuiteMetadata);

    for (const auto& [testClassName, testClassDescriptor] : testClassDescriptorsByClassName)
    {
        if (!testClassNames.empty() && !testClassNames.contains(testClassName))
        {
            continue;
        }

        const auto testClassResult = testClassDescriptor.runTestClass(testSuiteResult.testSuiteMetadata, reporter);

        testSuiteResult.testClassesRan += 1;
        testSuiteResult.testClassesPassed += testClassResult.testClassSuccess;
        testSuiteResult.testClassesFailed += !testClassResult.testClassSuccess;

        testSuiteResult.testMethodsRan += testClassResult.testMethodsRan;
        testSuiteResult.testMethodsPassed += testClassResult.testMethodsPassed;
        testSuiteResult.testMethodsFailed += testClassResult.testMethodsFailed;

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
