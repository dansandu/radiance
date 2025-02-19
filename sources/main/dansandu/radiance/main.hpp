#pragma once
#ifdef DANSANDU_RADIANCE_SUPPLY_DEFAULT_MAIN
#undef DANSANDU_RADIANCE_SUPPLY_DEFAULT_MAIN
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/progress_bar_console_reporter.hpp"
#include "dansandu/radiance/test_case_registry.hpp"
#include "dansandu/radiance/utility.hpp"

#include <iostream>
#include <string>
#include <vector>

using dansandu::radiance::exception::BaseRadianceException;
using dansandu::radiance::progress_bar_console_reporter::ProgressBarConsoleReporter;
using dansandu::radiance::test_case_registry::TestCaseRegistry;
using dansandu::radiance::utility::getEnvironmentVariable;
using dansandu::radiance::utility::toWideString;

int main(const int argc, const char* const* const argv)
{
    try
    {
        const auto stageIndexString = getEnvironmentVariable("PRALINE_PROGRESS_BAR_STAGE_INDEX");
        const auto stageIndex = stageIndexString.has_value() ? std::stoi(stageIndexString.value()) : 0;

        const auto stageCountString = getEnvironmentVariable("PRALINE_PROGRESS_BAR_STAGE_COUNT");
        const auto stageCount = stageCountString.has_value() ? std::stoi(stageCountString.value()) : 0;

        auto reporter = ProgressBarConsoleReporter{stageIndex, stageCount};

        if (argc > 1)
        {
            auto testCasesNames = std::vector<std::wstring>{};

            for (auto index = 1; index < argc; ++index)
            {
                testCasesNames.push_back(toWideString(argv[index]));
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
    catch (const BaseRadianceException& exception)
    {
        std::wcerr << "Exception was thrown with message: " << exception.message() << std::endl;
        return 1;
    }
    catch (const std::exception& exception)
    {
        std::cerr << "Exception was thrown with message: " << exception.what() << std::endl;
        return 1;
    }
}
#endif
