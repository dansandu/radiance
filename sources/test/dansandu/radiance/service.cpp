#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/logging.hpp"
#include "dansandu/journey/reporter.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/compound_reporter.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/progress_bar.hpp"
#include "dansandu/radiance/progress_bar_console_reporter.hpp"
#include "dansandu/radiance/test_class_registry.hpp"
#include "dansandu/radiance/test_reporter.hpp"
#include "dansandu/radiance/test_suite.hpp"
#include "dansandu/radiance/utility.hpp"
#include "dansandu/service_runner/service_registry.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

using dansandu::journey::Level;
using dansandu::journey::logging::Logger;
using dansandu::journey::logging::ReporterGuard;
using dansandu::journey::reporter::LogFileReporter;
using dansandu::journey::utility::getEnvironmentVariable;
using dansandu::journey::utility::writeToStandardError;
using dansandu::journey::utility::writeToStandardOutput;
using dansandu::radiance::compound_reporter::CompoundReporter;
using dansandu::radiance::progress_bar::ProgressBar;
using dansandu::radiance::progress_bar_console_reporter::ProgressBarConsoleReporter;
using dansandu::radiance::test_class_registry::TestClassRegistry;
using dansandu::radiance::test_reporter::TestReporter;
using dansandu::radiance::test_suite::runTestSuite;
using dansandu::radiance::utility::readFile;
using dansandu::radiance::utility::removeCarriage;
using dansandu::radiance::utility::TextHighlight;

namespace
{

const auto scenariosDirectory = std::filesystem::path{"resources/test/dansandu/radiance/scenario"};

const auto testReporterDirectoryName = "test_reporter";

const auto consoleReporterDirectoryName = "console_reporter";

// Delay printing the errors to the console otherwise the progress bar may erase them.
bool matchesBaseline(
    const std::string& reporterDirectoryName, const std::string& fileName, const std::string& result,
    std::string& errors
)
{
    const auto baselineFilePath = scenariosDirectory / reporterDirectoryName / fileName;

    if (std::filesystem::exists(baselineFilePath) && removeCarriage(readFile(baselineFilePath)) == result)
    {
        return true;
    }

    const auto outputFilePath = "target/temporary/actual_" + reporterDirectoryName + "_" + fileName;

    errors += dansandu::journey::utility::format(
        "Scenario baseline \"", baselineFilePath.generic_string(), "\" doesn't match. See \"", outputFilePath,
        "\" for actual output.\n"
    );

    auto file = std::ofstream{std::filesystem::path{outputFilePath}, std::ios_base::binary};
    file << std::noskipws << result;

    return false;
}

int runTestSuiteTestService(const int argumentCount, const char* const* const arguments)
{
    if (argumentCount < 2)
    {
        writeToStandardError("Please supply the library path from which to load the test classes.");

        return 1;
    }

    const auto reporterGuard = ReporterGuard{
        Logger::getGlobalInstance(), "RadianceTestSuiteLogFile", Level::debug, LogFileReporter{"unit_tests.log"}
    };

    auto scenariosReader = std::filesystem::directory_iterator(scenariosDirectory / testReporterDirectoryName);

    const auto scenarios = std::vector<std::filesystem::directory_entry>(
        std::filesystem::begin(scenariosReader), std::filesystem::end(scenariosReader)
    );

    const auto stageName = "test";
    const auto scenariosTotal = static_cast<int>(scenarios.size());
    const auto displayElapsedTime = true;

    std::optional<ProgressBar> progressBar(
        std::in_place, stageName, scenariosTotal, [](const auto& text) { writeToStandardOutput(text); },
        displayElapsedTime
    );

    const auto libraryName = arguments[1];

    const auto testClassDescriptorsByClassName =
        TestClassRegistry::getGlobalInstance().getTestClassDescriptors(libraryName);

    auto scenariosFailed = 0;
    auto scenariosPassed = 0;
    auto assertionsPassed = 0;

    auto errors = std::string{};

    for (const auto& entry : scenarios)
    {
        const auto fileExtension = entry.path().extension().string();
        const auto fileName = entry.path().filename().string();
        const auto testClassName = fileName.substr(0, fileName.size() - fileExtension.size());

        progressBar->updateDescription(testClassName);

        auto testReporter = TestReporter{};

        auto consoleOutput = std::string{};

        auto consoleReporter = ProgressBarConsoleReporter{
            [](const std::string&) {},
            [&](const std::string& text) { consoleOutput += text; },
            [](const std::string& text, const TextHighlight) { return text; },
        };

        auto compoundReporter = CompoundReporter{{testReporter, consoleReporter}};

        const auto loggingLevelFailure = Level::warning;

        runTestSuite({testClassName}, testClassDescriptorsByClassName, loggingLevelFailure, compoundReporter);

        const auto testReporterMatches =
            matchesBaseline(testReporterDirectoryName, fileName, testReporter.getString(), errors);

        const auto consoleReporterMatches =
            matchesBaseline(consoleReporterDirectoryName, fileName, consoleOutput, errors);

        if (!testReporterMatches || !consoleReporterMatches)
        {
            ++scenariosFailed;
        }
        else
        {
            ++scenariosPassed;
            assertionsPassed += 2;
        }

        progressBar->advance();
    }

    const auto scenariosSkipped = 0;

    const auto testMethodsPassed = scenariosPassed;

    progressBar->updateSummary(scenariosFailed, scenariosSkipped, scenariosPassed, testMethodsPassed, assertionsPassed);

    progressBar.reset();

    writeToStandardError(errors, /*flush*/ true);

    const auto returnCode = scenariosFailed == 0 ? 0 : 1;

    return returnCode;
}

}

DANSANDU_SERVICE_RUNNER_REGISTER_SERVICE("dansandu-radiance-radiance_scenarios", runTestSuiteTestService);
