#include "dansandu/radiance/progress_bar_console_reporter.hpp"
#include "dansandu/radiance/utility.hpp"

#include <ios>
#include <iostream>
#include <type_traits>

using dansandu::radiance::progress_bar::ProgressBar;
using dansandu::radiance::utility::join;

namespace dansandu::radiance::progress_bar_console_reporter
{

ProgressBarConsoleReporter::ProgressBarConsoleReporter() : failureHandled_{false}
{
    stream_ << std::boolalpha;
}

void ProgressBarConsoleReporter::testSuiteBegin(const TestSuiteMetadata& metadata)
{
    const auto stageIndex = 12;
    const auto stageCount = 12;
    const auto stageName = L"test";
    const auto resolution = metadata.testCasesBeingRun;
    const auto displayElapsedTime = true;

    progressBar_ =
        ProgressBar{stageIndex,        stageCount, stageName, resolution, [](const auto& text) { std::wcout << text; },
                    displayElapsedTime};
}

void ProgressBarConsoleReporter::testSuiteEnd(const TestSuiteResult& result)
{
    progressBar_->updateSummary(result.testCasesFailed, result.testSuiteMetadata.testCasesBeingSkipped,
                                result.testCasesPassed);
    progressBar_.reset();

    const auto output = stream_.str();

    if (!output.empty())
    {
        std::wcout << output;
    }
}

void ProgressBarConsoleReporter::testCaseBegin(const TestCaseMetadata& metadata)
{
    progressBar_->updateDescription(metadata.testCaseName);
}

void ProgressBarConsoleReporter::testCaseEnd(const TestCaseResult& result)
{
    progressBar_->advance(1);
}

void ProgressBarConsoleReporter::testCaseRunBegin(const TestCaseRunMetadata& metadata)
{
    failureHandled_ = false;
}

void ProgressBarConsoleReporter::testCaseRunEnd(const TestCaseRunResult& result)
{
    if (!failureHandled_)
    {
        if (result.exceptionMetadata)
        {
            const auto& testCaseMetadata = result.testCaseRunMetadata.testCaseMetadata;

            stream_ << "  Test case failed " << testCaseMetadata.filePath << "(" << testCaseMetadata.lineNumber << ")"
                    << std::endl
                    << "    within test case \"" << testCaseMetadata.testCaseName
                    << "\" exception was thrown with message: \"" << result.exceptionMetadata->exceptionMessage << '"'
                    << std::endl
                    << std::endl;
        }
    }
}

void ProgressBarConsoleReporter::assertionEnd(const AssertionResult& result)
{
    if (!result.assertionSuccess)
    {
        failureHandled_ = true;

        const auto& assertionMetadata = result.assertionMetadata;
        const auto& sectionMetadata = assertionMetadata.sectionMetadata;
        const auto& testCaseMetadata = sectionMetadata.testCaseRunMetadata.testCaseMetadata;

        stream_ << "  Assertion failed " << testCaseMetadata.filePath << "(" << assertionMetadata.lineNumber << ")"
                << std::endl;

        if (!sectionMetadata.sections.empty())
        {
            stream_ << "    within test case \"" << testCaseMetadata.testCaseName << "\" sections \""
                    << join(sectionMetadata.sections, L"\" -> \"") << '"' << std::endl;
        }
        else
        {
            stream_ << "    within test case \"" << testCaseMetadata.testCaseName << '"' << std::endl;
        }

        stream_ << "      REQUIRE(" << assertionMetadata.expression << ')' << std::endl;

        if (!result.exceptionMetadata)
        {
            std::visit(
                [&](auto&& argument)
                {
                    using ArgumentType = std::decay_t<decltype(argument)>;

                    if constexpr (std::is_same_v<ArgumentType, UnaryAssertion>)
                    {
                        stream_ << "              " << argument.representation << std::endl
                                << "      evaluates to " << result.assertionSuccess << std::endl
                                << std::endl;
                    }
                    else if constexpr (std::is_same_v<ArgumentType, BinaryAssertion>)
                    {
                        stream_ << "              " << argument.firstRepresentation << ' ' << argument.operation << ' '
                                << argument.secondRepresentation << std::endl
                                << "      evaluates to " << result.assertionSuccess << std::endl
                                << std::endl;
                    }
                    else
                    {
                        static_assert(false, "Assertion visitor is non-exhaustive");
                    }
                },
                result.assertion);
        }
        else
        {
            stream_ << "      exception was thrown with message: \"" << result.exceptionMetadata->exceptionMessage
                    << '"' << std::endl
                    << std::endl;
        }
    }
}

}
