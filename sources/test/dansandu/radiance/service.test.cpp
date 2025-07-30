#include "dansandu/journey/exception.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/progress_bar.hpp"
#include "dansandu/radiance/test_case_registry.hpp"
#include "dansandu/radiance/test_reporter.test.hpp"
#include "dansandu/radiance/utility.hpp"
#include "dansandu/service_runner/service_registry.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

using dansandu::radiance::progress_bar::ProgressBar;
using dansandu::radiance::test_case_registry::TestCaseRegistry;
using dansandu::radiance::test_reporter::TestReporter;
using dansandu::radiance::utility::getEnvironmentVariable;
using dansandu::radiance::utility::readFile;
using dansandu::radiance::utility::removeCarriage;

namespace
{

int runScenarios(const int, const char* const* const)
{
    const auto stageIndexString = getEnvironmentVariable("PRALINE_PROGRESS_BAR_STAGE_INDEX");
    const auto stageIndex = stageIndexString.has_value() ? std::stoi(stageIndexString.value()) : 0;

    const auto stageCountString = getEnvironmentVariable("PRALINE_PROGRESS_BAR_STAGE_COUNT");
    const auto stageCount = stageCountString.has_value() ? std::stoi(stageCountString.value()) : 0;

    const auto scenariosDirectory = "resources/test/dansandu/radiance/scenario";

    auto scenariosReader = std::filesystem::directory_iterator(scenariosDirectory);

    const auto scenarios = std::vector<std::filesystem::directory_entry>(std::filesystem::begin(scenariosReader),
                                                                         std::filesystem::end(scenariosReader));

    const auto stageName = L"test";
    const auto scenariosTotal = static_cast<int>(scenarios.size());
    const auto displayElapsedTime = true;

    std::optional<ProgressBar> progressBar(
        std::in_place, stageIndex, stageCount, stageName, scenariosTotal, [](const auto& text) { std::wcout << text; },
        displayElapsedTime);

    auto scenariosFailed = 0;
    auto scenariosPassed = 0;
    auto assertionsPassed = 0;

    for (const auto& entry : scenarios)
    {
        const auto fileExtension = std::wstring{entry.path().extension()};
        const auto fileName = std::wstring{entry.path().filename()};
        const auto testCaseName = fileName.substr(0, fileName.size() - fileExtension.size());
        const auto expectedOutput = removeCarriage(readFile(entry.path()));

        progressBar->updateDescription(testCaseName);

        auto reporter = TestReporter{};

        TestCaseRegistry::instance().runTestCases({testCaseName}, reporter);

        const auto output = reporter.getString();

        if (output != expectedOutput)
        {
            ++scenariosFailed;

            const auto scenariosSkipped = scenariosTotal - scenariosPassed - scenariosFailed;

            progressBar->updateSummary(scenariosFailed, scenariosSkipped, scenariosPassed, assertionsPassed);
            progressBar.reset();

            const auto outputFilePath = L"target/temporary/actual_" + std::wstring{entry.path().filename()};

            std::wcout << "Scenario \"" << testCaseName << "\" failed. See \"" << outputFilePath
                       << "\" for actual output." << std::endl;

            auto file = std::wofstream{outputFilePath, std::ios_base::binary};
            file << std::noskipws << output;

            return 1;
        }
        else
        {
            ++scenariosPassed;
            ++assertionsPassed;
        }

        progressBar->advance();
    }

    const auto scenariosSkipped = scenariosTotal - scenariosPassed;

    progressBar->updateSummary(scenariosFailed, scenariosSkipped, scenariosPassed, assertionsPassed);

    return 0;
}

}

DANSANDU_SERVICE_RUNNER_REGISTER_SERVICE("radiance_scenarios", runScenarios);
