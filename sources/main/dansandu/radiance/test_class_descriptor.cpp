#include "dansandu/radiance/test_class_descriptor.hpp"
#include "dansandu/journey/logging.hpp"
#include "dansandu/journey/reporter.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/base_test_class.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/test_method_context.hpp"
#include "dansandu/radiance/utility.hpp"

#include <exception>
#include <memory>
#include <optional>
#include <string>
#include <vector>

using dansandu::journey::logging::Logger;
using dansandu::journey::logging::ReporterGuard;
using dansandu::journey::reporter::InMemoryReporter;
using dansandu::journey::utility::getTypeName;
using dansandu::radiance::base_test_class::BaseTestClass;
using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::test_method_context::TestMethodContext;
using dansandu::radiance::utility::journeyLogsToRadianceLogs;

namespace dansandu::radiance::test_class_descriptor
{

namespace
{

template<typename Callable>
std::optional<ExceptionMetadata> captureException(Callable&& callable)
{
    try
    {
        callable();

        return std::nullopt;
    }
    catch (const std::exception& exception)
    {
        return ExceptionMetadata{
            .exceptionType = getTypeName(/*includeNamespace*/ true, exception),
            .exceptionMessage = exception.what(),
        };
    }
    catch (...)
    {
        return ExceptionMetadata{
            .exceptionType = "<Unknown>",
            .exceptionMessage = "<Unknown>",
        };
    }
}

}

TestClassDescriptor::TestClassDescriptor(
    const std::string& testClassName, const std::string& testClassFilePath, const int testClassLineNumber,
    const TestClassProducer testClassProducer, const SubclassAddressResolver subclassAddressResolver,
    const std::vector<TestMethodDescriptor>& testMethodDescriptors
)
    : testClassName_{testClassName},
      testClassFilePath_{testClassFilePath},
      testClassLineNumber_{testClassLineNumber},
      testClassProducer_{testClassProducer},
      subclassAddressResolver_{subclassAddressResolver},
      testMethodDescriptors_{testMethodDescriptors}
{
}

TestClassResult TestClassDescriptor::runTestClass(const TestSuiteMetadata& testSuiteMetadata, IReporter& reporter) const
{
    auto testClassResult = TestClassResult{
        .testClassMetadata =
            TestClassMetadata{
                .testSuiteMetadata = testSuiteMetadata,
                .testClassName = testClassName_,
                .testClassFilePath = testClassFilePath_,
                .testClassLineNumber = testClassLineNumber_,
            },
        .testMethodsRan = 0,
        .testMethodsPassed = 0,
        .testMethodsFailed = 0,
        .assertionsRan = 0,
        .assertionsPassed = 0,
        .assertionsFailed = 0,
        .loggingSuccess = true,
        .testClassSuccess = true,
    };

    reporter.testClassBegin(testClassResult.testClassMetadata);

    for (const auto& testMethodDescriptor : testMethodDescriptors_)
    {
        auto testMethodResult = TestMethodResult{
            .testMethodMetadata =
                {
                    .testClassMetadata = testClassResult.testClassMetadata,
                    .testMethodName = testMethodDescriptor.testMethodName,
                    .testMethodLineNumber = testMethodDescriptor.testMethodLineNumber,
                },
            .assertionsRan = 0,
            .assertionsPassed = 0,
            .assertionsFailed = 0,
            .failingLogs = {},
            .loggingSuccess = false,
            .setUpStatus = StageStatus::NotRun,
            .cleanUpStatus = StageStatus::NotRun,
            .testMethodSuccess = false,
            .exceptionMetadata = std::nullopt,
            .cleanUpExceptionMetadata = std::nullopt,
        };

        LOG_DEBUG("Beginning test method '", testClassName_, "::", testMethodDescriptor.testMethodName, "'");

        reporter.testMethodBegin(testMethodResult.testMethodMetadata);

        const auto reporterName = "RadianceLogger." + testClassName_ + "." + testMethodDescriptor.testMethodName;
        const auto reporterLevel =
            testMethodResult.testMethodMetadata.testClassMetadata.testSuiteMetadata.loggingLevelFailure;
        const auto inMemoryReporter = InMemoryReporter{};
        const auto reporterGuard =
            ReporterGuard{Logger::getGlobalInstance(), reporterName, reporterLevel, inMemoryReporter};

        auto testMethodContext = TestMethodContext{testMethodResult, inMemoryReporter, reporter};

        {
            auto testClass = std::unique_ptr<BaseTestClass>{};

            testMethodResult.exceptionMetadata = captureException(
                [&]()
                {
                    testClass = testClassProducer_();

                    testClass->setTestMethodContext(testMethodContext);
                }
            );

            if (!testMethodResult.exceptionMetadata)
            {
                const auto assertionsFailedBeforeSetup = testMethodResult.assertionsFailed;

                testMethodResult.exceptionMetadata = captureException([&]() { testClass->setUp(); });

                testMethodResult.setUpStatus = testMethodResult.exceptionMetadata ||
                                                       testMethodResult.assertionsFailed != assertionsFailedBeforeSetup
                                                   ? StageStatus::Failed
                                                   : StageStatus::Succeeded;
            }

            if (testMethodResult.setUpStatus == StageStatus::Succeeded)
            {
                testMethodResult.exceptionMetadata =
                    captureException([&]()
                                     { testMethodDescriptor.testMethodInvoker(subclassAddressResolver_(*testClass)); });

                const auto assertionsFailedBeforeCleanup = testMethodResult.assertionsFailed;

                testMethodResult.cleanUpExceptionMetadata = captureException([&]() { testClass->cleanUp(); });

                testMethodResult.cleanUpStatus =
                    testMethodResult.cleanUpExceptionMetadata ||
                            testMethodResult.assertionsFailed != assertionsFailedBeforeCleanup
                        ? StageStatus::Failed
                        : StageStatus::Succeeded;
            }

            testMethodResult.testMethodSuccess = !testMethodResult.exceptionMetadata &&
                                                 testMethodResult.assertionsFailed == 0 &&
                                                 testMethodResult.cleanUpStatus == StageStatus::Succeeded;
        }

        testMethodResult.failingLogs = journeyLogsToRadianceLogs(inMemoryReporter.getLoggedEntries());

        testMethodResult.loggingSuccess = testMethodResult.failingLogs.empty();

        testMethodResult.testMethodSuccess &= testMethodResult.loggingSuccess;

        reporter.testMethodEnd(testMethodResult);

        LOG_DEBUG("Ending test method '", testClassName_, "::", testMethodDescriptor.testMethodName, "'");

        testClassResult.testMethodsRan += 1;
        testClassResult.testMethodsPassed += testMethodResult.testMethodSuccess;
        testClassResult.testMethodsFailed += !testMethodResult.testMethodSuccess;

        testClassResult.assertionsRan += testMethodResult.assertionsRan;
        testClassResult.assertionsPassed += testMethodResult.assertionsPassed;
        testClassResult.assertionsFailed += testMethodResult.assertionsFailed;

        testClassResult.loggingSuccess &= testMethodResult.loggingSuccess;
        testClassResult.testClassSuccess &= testMethodResult.testMethodSuccess;
    }

    reporter.testClassEnd(testClassResult);

    return testClassResult;
}

}
