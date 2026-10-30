#include "dansandu/radiance/test_class_descriptor.hpp"
#include "dansandu/radiance/test_method_context.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/journey/reporter.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/journey/logging.hpp"

#include <string>
#include <vector>

using dansandu::journey::utility::getTypeName;
using dansandu::journey::logging::Logger;
using dansandu::journey::logging::ReporterGuard;
using dansandu::journey::reporter::InMemoryReporter;
using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::test_method_context::TestMethodContext;

namespace dansandu::radiance::test_class_descriptor
{

TestClassDescriptor::TestClassDescriptor(
        const std::string& testClassName,
        const std::string& testClassFilePath,
        const int testClassLineNumber,
        const TestClassProducer testClassProducer,
        std::vector<TestMethodDescriptor>&& testMethodDescriptors
    )
  : testClassName_{testClassName},
    testClassFilePath_{testClassFilePath},
    testClassLineNumber_{testClassLineNumber},
    testClassProducer_{testClassProducer},
    testMethodDescriptors_{testMethodDescriptors}
{

}

TestClassResult TestClassDescriptor::runTestClass(const TestSuiteMetadata& testSuiteMetadata, IReporter& reporter) const
{
    auto testClassResult = TestClassResult{
        .testClassMetadata = TestClassMetadata{
            .testSuiteMetadata = testSuiteMetadata,
            .testClassName = testClassName_,
            .testClassFilePath = testClassFilePath_,
            .testClassLineNumber = testClassLineNumber_,
        },
        .assertionsRan = 0,
        .assertionsPassed = 0,
        .assertionsFailed = 0,
        .loggingSuccess = true,
        .testClassSuccess = true,
    };

    reporter.testClassBegin(testClassResult.testClassMetadata);

    for (const auto testMethodDescriptor : testMethodDescriptors_)
    {
        auto testMethodResult = TestMethodResult{
            .testMethodMetadata = {
                .testClassMetadata = testClassResult.testClassMetadata,
                .testMethodName = testMethodDescriptor.testMethodName,
                .testMethodLineNumber = testMethodDescriptor.testMethodLineNumber,
            },
            .assertionsRan = 0,
            .assertionsPassed = 0,
            .assertionsFailed = 0,
            .failingLogs = {},
            .loggingSuccess = false,
            .testMethodSuccess = false,
            .exceptionMetadata = std::nullopt,
        },

        reporter.testMethodBegin(testMethodResult.testMethodMetadata);

        const auto testMethodContext = TestMethodContext{testMethodResult, reporter};

        const auto reporterName = "RadianceLogger." + testMethodResult.testClassMetadata.testClassName + "." + testMethodResult.testMethodName;
        const auto reporterLevel = testMethodResult.testClassMetadata.testSuiteMetadata.loggingLevelFailure;
        const auto inMemoryReporter = InMemoryReporter{};
        const auto reporterGuard = ReporterGuard{Logger::getGlobalInstance(), reporterName, reporterLevel, inMemoryReporter};

        try
        {
            const auto testClass = testClassProducer_.testClassProducer();

            testClass.setTestMethodContext(testMethodContext);

            testClass.setUp();

            testClass.invokeTestMethod();

            testClass.cleanUp();

            testMethodResult.testMethodSuccess = true;
        }
        catch (const std::exception& exception)
        {
            testMethodResult.exceptionMetadata = ExceptionMetadata{
                .exceptionType = getTypeName(/*includeNamespace*/ true, exception),
                .exceptionMessage = exception.what(),
            };
        }
        catch (...)
        {
            testMethodResult.exceptionMetadata = ExceptionMetadata{
                .exceptionType = "<Unknown>",
                .exceptionMessage = "<Unknown>",
            };
        }

        testMethodResult.failingLogs = journeyLogsToRadianceLogs(inMemoryReporter.getLoggedEntries());

        testMethodResult.loggingSuccess = testMethodResult.failingLogs.empty();

        testMethodResult.testMethodSuccess &= testMethodResult.loggingSuccess;

        reporter.testMethodEnd(testMethodResult);

        testClassResult.assertionsRan += testMethodResult.assertionsRan;

        testClassResult.assertionsPassed += testMethodResult.assertionsPassed;

        testClassResult.assertionsFailed += testMethodResult.assertionsFailed;

        testClassResult.loggingSuccess &= testMethodResult.loggingSuccess;

        testClassResult.testClassSuccess &= testMethodResult.testCaseRunSuccess;
    }

    reporter.testClassEnd(testClassResult);

    return testClassResult;
}

}
