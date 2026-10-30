#include "dansandu/radiance/progress_bar_console_reporter.hpp"
#include "dansandu/journey/common.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/utility.hpp"

#include <ios>
#include <iostream>
#include <ranges>
#include <type_traits>

using dansandu::journey::toString;
using dansandu::journey::toStringWithConsoleHighlight;
using dansandu::journey::utility::writeToStandardOutput;
using dansandu::radiance::progress_bar::ProgressBar;
using dansandu::radiance::utility::highlightText;
using dansandu::radiance::utility::TextHighlight;

namespace dansandu::radiance::progress_bar_console_reporter
{

constexpr auto logLimit = 5uz;

ProgressBarConsoleReporter::ProgressBarConsoleReporter() : failureHandled_{false}
{
    stream_ << std::boolalpha;
}

void ProgressBarConsoleReporter::testSuiteBegin(const TestSuiteMetadata& testSuiteMetadata)
{
    const auto stageName = "test";
    const auto resolution = testSuiteMetadata.testClassesToRun;
    const auto displayElapsedTime = true;

    progressBar_.emplace(stageName, resolution, [](const auto& text) { writeToStandardOutput(text, /*flush*/ true); }, displayElapsedTime);
}

void ProgressBarConsoleReporter::testSuiteEnd(const TestSuiteResult& testSuiteResult)
{
    progressBar_->updateSummary(testSuiteResult.testClassesFailed, testSuiteResult.testSuiteMetadata.testClassesToSkip,
                                testSuiteResult.testClassesPassed, testSuiteResult.assertionsPassed);
    progressBar_.reset();

    const auto output = stream_.str();

    if (!output.empty())
    {
        writeToStandardOutput(output, /*flush*/ true);
    }
}

void ProgressBarConsoleReporter::testClassBegin(const TestClassMetadata& testClassMetadata)
{
    progressBar_->updateDescription(testClassMetadata.testClassName);
}

void ProgressBarConsoleReporter::testClassEnd(const TestClassResult&)
{
    progressBar_->advance();
}

void ProgressBarConsoleReporter::testMethodBegin(const TestMethodMetadata&)
{
    failureHandled_ = false;
}

namespace
{

void printLogs(std::ostringstream& stream, const std::vector<Log>& logs, size_t logLimit)
{
    for (auto index = 0uz; index < std::min(logs.size(), logLimit); ++index)
    {
        stream << toStringWithConsoleHighlight(logs[index].level) << " " << logs[index].message << std::endl;
    }
}

}

void ProgressBarConsoleReporter::testMethodEnd(const TestMethodResult& testMethodResult)
{
    if (!failureHandled_)
    {
        const auto& testClassMetadata = testMethodResult.testMethodMetadata.testClassMetadata;

        if (testMethodResult.exceptionMetadata)
        {
            stream_ << "  " << highlightText("Test class failed", TextHighlight::Red) << " "
                    << testClassMetadata.testClassFilePath.c_str() << "(" << testClassMetadata.testClassLineNumber << ")" << std::endl
                    << "    within test class "
                    << highlightText(testClassMetadata.testClassName, TextHighlight::Magenta)
                    << " exception was thrown with message: \"" << testMethodResult.exceptionMetadata->exceptionMessage << '"'
                    << std::endl
                    << std::endl;
        }
        else if (!testMethodResult.loggingSuccess)
        {
            stream_ << "  " << highlightText("Test class failed", TextHighlight::Red) << " "
                    << testClassMetadata.testClassFilePath.c_str() << "(" << testClassMetadata.testClassLineNumber << ")" << std::endl
                    << "    within test class "
                    << highlightText(testClassMetadata.testClassName, TextHighlight::Magenta) << " "
                    << toStringWithConsoleHighlight(testClassMetadata.testSuiteMetadata.loggingLevelFailure);

            if (testMethodResult.failingLogs.size() > logLimit)
            {
                stream_ << "(s) or above were logged (displaying only the first " << logLimit << "):" << std::endl;
            }
            else
            {
                stream_ << "(s) or above were logged:" << std::endl;
            }

            printLogs(stream_, testMethodResult.failingLogs, logLimit);

            stream_ << std::endl;
        }
    }
}

void ProgressBarConsoleReporter::assertionEnd(const AssertionResult& assertionResult)
{
    if (!assertionResult.assertionSuccess)
    {
        failureHandled_ = true;

        const auto& assertionMetadata = assertionResult.assertionMetadata;
        const auto& testClassMetadata = assertionMetadata.testMethodMetadata.testClassMetadata;

        stream_ << "  " << highlightText("Assertion failed", TextHighlight::Red) << " "
                << testClassMetadata.testClassFilePath.c_str() << "(" << assertionMetadata.assertionLineNumber << ")" << std::endl;

        if (!sectionMetadata.sections.empty())
        {
            const auto sections = getFormattedSectionsCallStack(sectionMetadata.sections);

            stream_ << "    within test class "
                    << highlightText(testClassMetadata.testClassName, TextHighlight::Magenta) << " sections "
                    << sections << std::endl;
        }
        else
        {
            stream_ << "    within test class "
                    << highlightText(testClassMetadata.testClassName, TextHighlight::Magenta) << std::endl;
        }

        if (!assertionResult.exceptionMetadata)
        {
            std::visit(
                [&](auto&& argument)
                {
                    using ArgumentType = std::decay_t<decltype(argument)>;

                    if constexpr (std::is_same_v<ArgumentType, UnaryAssertion>)
                    {
                        stream_ << "      " << highlightText("REQUIRE", TextHighlight::Blue) << "("
                                << assertionMetadata.expression.c_str() << ')' << std::endl
                                << "              " << argument.representation << std::endl
                                << "      evaluates to " << assertionResult.assertionSuccess << std::endl
                                << std::endl;
                    }
                    else if constexpr (std::is_same_v<ArgumentType, BinaryAssertion>)
                    {
                        stream_ << "      " << highlightText("REQUIRE", TextHighlight::Blue) << "("
                                << assertionMetadata.expression.c_str() << ')' << std::endl
                                << "              " << argument.firstRepresentation << ' ' << argument.operation.c_str()
                                << ' ' << argument.secondRepresentation << std::endl
                                << "      evaluates to " << assertionResult.assertionSuccess << std::endl
                                << std::endl;
                    }
                    else if constexpr (std::is_same_v<ArgumentType, ThrowAssertion>)
                    {
                        stream_ << "      " << highlightText("REQUIRE_THROW", TextHighlight::Blue) << "("
                                << assertionMetadata.expression.c_str() << ')' << std::endl;

                        if (!argument.exceptionThrown)
                        {
                            stream_ << "      no exception was thrown";
                        }
                        else
                        {
                            stream_ << "      a different type of exception was thrown with message:\n"
                                    << "            " << argument.actualException.c_str() << ": \""
                                    << argument.exceptionMessage << '"' << std::endl;
                        }
                    }
                    else if constexpr (std::is_same_v<ArgumentType, LogAssertion>)
                    {
                        stream_ << "      " << highlightText("REQUIRE_LOGGING", TextHighlight::Blue) << "("
                                << assertionMetadata.expression.c_str() << ')' << std::endl;

                        if (argument.expectedLogs.size() > logLimit)
                        {
                            stream_ << "      mismatch between expected logs (displaying only the first " << logLimit
                                    << "):" << std::endl;
                        }
                        else
                        {
                            stream_ << "      mismatch between expected logs:" << std::endl << std::endl;
                        }

                        printLogs(stream_, argument.expectedLogs, logLimit);

                        if (argument.actualLogs.size() > logLimit)
                        {
                            stream_ << std::endl
                                    << "      and actual logs (displaying only the first " << logLimit
                                    << "):" << std::endl
                                    << std::endl;
                        }
                        else
                        {
                            stream_ << std::endl << "      and actual logs:" << std::endl << std::endl;
                        }

                        printLogs(stream_, argument.actualLogs, logLimit);

                        stream_ << std::endl;
                    }
                    else
                    {
                        static_assert(false, "Assertion visitor is non-exhaustive");
                    }
                },
                assertionResult.assertion);
        }
        else
        {
            stream_ << "      exception was thrown with message: \""
                    << assertionResult.exceptionMetadata->exceptionMessage << '"' << std::endl
                    << std::endl;
        }
    }
}

}
