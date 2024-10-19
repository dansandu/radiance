#include "dansandu/radiance/test_case_registry.hpp"
#include "dansandu/radiance/test_reporter.test.hpp"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

using dansandu::radiance::test_case_registry::TestCaseRegistry;
using dansandu::radiance::test_reporter::TestReporter;

static std::wstring readFile(const std::filesystem::path& filePath)
{
    auto file = std::wifstream{filePath, std::ios_base::binary};
    file >> std::noskipws;
    auto stream = std::wostringstream{};
    stream << file.rdbuf();
    return stream.str();
}

static std::wstring removeCarriage(std::wstring text)
{
    text.erase(std::remove(text.begin(), text.end(), L'\r'), text.end());
    return text;
}

int main(const int, const char* const* const)
{
    const auto scenarioDirectory = "resources/test/dansandu/radiance/scenario";

    for (const auto& entry : std::filesystem::directory_iterator(scenarioDirectory))
    {
        const auto fileExtension = std::wstring{entry.path().extension()};
        const auto fileName = std::wstring{entry.path().filename()};
        const auto testCaseName = fileName.substr(0, fileName.size() - fileExtension.size());
        const auto expectedOutput = removeCarriage(readFile(entry.path()));

        auto reporter = TestReporter{};

        TestCaseRegistry::instance().runTestCase(testCaseName, reporter);

        const auto output = reporter.getString();

        if (output != expectedOutput)
        {
            const auto outputFilePath = L"target/temporary/actual_" + std::wstring{entry.path().filename()};

            std::wcout << "Scenario \"" << testCaseName << "\" failed. See \"" << outputFilePath
                       << "\" for actual output." << std::endl;

            auto file = std::wofstream{outputFilePath, std::ios_base::binary};
            file << std::noskipws << output;

            return 1;
        }
    }

    return 0;
}
