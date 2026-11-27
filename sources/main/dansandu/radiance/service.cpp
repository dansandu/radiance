#include "dansandu/journey/logging.hpp"
#include "dansandu/journey/reporter.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/progress_bar_console_reporter.hpp"
#include "dansandu/radiance/test_class_registry.hpp"
#include "dansandu/radiance/test_suite.hpp"
#include "dansandu/radiance/utility.hpp"
#include "dansandu/service_runner/service_registry.hpp"

#include <set>
#include <string>

using dansandu::journey::Level;
using dansandu::journey::logging::Logger;
using dansandu::journey::logging::ReporterGuard;
using dansandu::journey::reporter::LogFileReporter;
using dansandu::journey::utility::writeToStandardOutput;
using dansandu::radiance::progress_bar_console_reporter::ProgressBarConsoleReporter;
using dansandu::radiance::test_class_registry::TestClassRegistry;
using dansandu::radiance::test_suite::runTestSuite;
using dansandu::radiance::utility::highlightText;

namespace
{

int runTestSuiteMainService(const int argumentCount, const char* const* const arguments)
{
    const auto reporterGuard = ReporterGuard{
        Logger::getGlobalInstance(), "RadianceTestSuiteLogFile", Level::debug, LogFileReporter{"unit_tests.log"}
    };

    const auto printer = [](const std::string& text) { writeToStandardOutput(text, /*flush*/ true); };

    auto reporter = ProgressBarConsoleReporter{printer, printer, highlightText};

    const auto libraryName = arguments[1];

    const auto testClassDescriptorsByClassName =
        TestClassRegistry::getGlobalInstance().getTestClassDescriptors(libraryName);

    const auto loggingLevelFailure = Level::warning;

    auto testClassNames = std::set<std::string>{};

    for (auto index = 3; index < argumentCount; ++index)
    {
        testClassNames.insert(arguments[index]);
    }

    const auto testSuiteResult =
        runTestSuite(testClassNames, testClassDescriptorsByClassName, loggingLevelFailure, reporter);

    const auto returnCode = !testSuiteResult.testSuiteSuccess;

    return returnCode;
}

}

DANSANDU_SERVICE_RUNNER_REGISTER_SERVICE("dansandu-radiance-run_tests", runTestSuiteMainService);
