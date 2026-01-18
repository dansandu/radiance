#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/progress_bar.hpp"
#include "dansandu/radiance/test_case_registry.hpp"
#include "dansandu/radiance/test_reporter.hpp"
#include "dansandu/radiance/utility.hpp"
#include "dansandu/service_runner/service_registry.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

using dansandu::journey::utility::getEnvironmentVariable;
using dansandu::radiance::progress_bar::ProgressBar;
using dansandu::radiance::test_case_registry::TestCaseRegistry;
using dansandu::radiance::test_reporter::TestReporter;
using dansandu::radiance::utility::readFile;
using dansandu::radiance::utility::removeCarriage;

namespace
{

int runScenarios(const int, const char* const* const)
{
    const auto scenariosDirectory = "resources/test/dansandu/radiance/scenario";

    auto scenariosReader = std::filesystem::directory_iterator(scenariosDirectory);

    const auto scenarios = std::vector<std::filesystem::directory_entry>(std::filesystem::begin(scenariosReader),
                                                                         std::filesystem::end(scenariosReader));

    const auto stageName = L"test";
    const auto scenariosTotal = static_cast<int>(scenarios.size());
    const auto displayElapsedTime = true;

    std::optional<ProgressBar> progressBar(
        std::in_place, stageName, scenariosTotal, [](const auto& text) { std::wcout << text; }, displayElapsedTime);

    auto scenariosFailed = 0;
    auto scenariosPassed = 0;
    auto assertionsPassed = 0;

    for (const auto& entry : scenarios)
    {
        const auto fileExtension = entry.path().extension().wstring();
        const auto fileName = entry.path().filename().wstring();
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

            const auto outputFilePath = L"target/temporary/actual_" + entry.path().filename().wstring();

            std::wcout << "Scenario \"" << testCaseName << "\" failed. See \"" << outputFilePath
                       << "\" for actual output." << std::endl;

            auto file = std::wofstream{std::filesystem::path{outputFilePath}, std::ios_base::binary};
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

DANSANDU_SERVICE_RUNNER_REGISTER_SERVICE("dansandu-radiance-radiance_scenarios", runScenarios);
