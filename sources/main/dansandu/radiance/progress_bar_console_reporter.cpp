#include "dansandu/radiance/progress_bar_console_reporter.hpp"
#include "dansandu/radiance/utility.hpp"

#include <ios>
#include <iostream>
#include <ranges>
#include <type_traits>

using dansandu::radiance::progress_bar::ProgressBar;
using dansandu::radiance::utility::highlightText;
using dansandu::radiance::utility::join;
using dansandu::radiance::utility::TextHighlight;

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
                                result.testCasesPassed, result.assertionsPassed);
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

            stream_ << "  " << highlightText(L"Test case failed", TextHighlight::Red) << " "
                    << testCaseMetadata.filePath << "(" << testCaseMetadata.lineNumber << ")" << std::endl
                    << "    within test case " << highlightText(testCaseMetadata.testCaseName, TextHighlight::Magenta)
                    << " exception was thrown with message: \"" << result.exceptionMetadata->exceptionMessage << '"'
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

        stream_ << "  " << highlightText(L"Assertion failed", TextHighlight::Red) << " " << testCaseMetadata.filePath
                << "(" << assertionMetadata.lineNumber << ")" << std::endl;

        if (!sectionMetadata.sections.empty())
        {
            const auto sections =
                join(sectionMetadata.sections |
                         std::views::transform([](const auto& section)
                                               { return highlightText(section, TextHighlight::Magenta); }),
                     L" -> ");

            stream_ << "    within test case " << highlightText(testCaseMetadata.testCaseName, TextHighlight::Magenta)
                    << " sections " << sections << std::endl;
        }
        else
        {
            stream_ << "    within test case " << highlightText(testCaseMetadata.testCaseName, TextHighlight::Magenta)
                    << std::endl;
        }

        if (!result.exceptionMetadata)
        {
            std::visit(
                [&](auto&& argument)
                {
                    using ArgumentType = std::decay_t<decltype(argument)>;

                    if constexpr (std::is_same_v<ArgumentType, UnaryAssertion>)
                    {
                        stream_ << "      " << highlightText(L"REQUIRE", TextHighlight::Blue) << "("
                                << assertionMetadata.expression << ')' << std::endl
                                << "              " << argument.representation << std::endl
                                << "      evaluates to " << result.assertionSuccess << std::endl
                                << std::endl;
                    }
                    else if constexpr (std::is_same_v<ArgumentType, BinaryAssertion>)
                    {
                        stream_ << "      " << highlightText(L"REQUIRE", TextHighlight::Blue) << "("
                                << assertionMetadata.expression << ')' << std::endl
                                << "              " << argument.firstRepresentation << ' ' << argument.operation << ' '
                                << argument.secondRepresentation << std::endl
                                << "      evaluates to " << result.assertionSuccess << std::endl
                                << std::endl;
                    }
                    else if constexpr (std::is_same_v<ArgumentType, ThrowAssertion>)
                    {
                        stream_ << "      " << highlightText(L"REQUIRE_THROW", TextHighlight::Blue) << "("
                                << assertionMetadata.expression << ')' << std::endl;

                        if (!argument.exceptionThrown)
                        {
                            stream_ << "      no exception was thrown";
                        }
                        else
                        {
                            stream_ << "      a different type of exception was thrown with message: \""
                                    << argument.exceptionMessage << '"' << std::endl;
                        }
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
