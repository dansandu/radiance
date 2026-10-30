#include "dansandu/radiance/progress_bar_console_reporter.hpp"
#include "dansandu/journey/common.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/utility.hpp"

#include <ios>
#include <iostream>
#include <ranges>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

using dansandu::journey::Level;
using dansandu::journey::toString;
using dansandu::radiance::progress_bar::ProgressBar;
using dansandu::radiance::utility::TextHighlight;

namespace dansandu::radiance::progress_bar_console_reporter
{

constexpr auto logLimit = 5uz;

ProgressBarConsoleReporter::ProgressBarConsoleReporter(
    Printer progressBarPrinter, Printer outputPrinter, Highlighter highlighter
)
    : progressBarPrinter_{std::move(progressBarPrinter)},
      outputPrinter_{std::move(outputPrinter)},
      highlighter_{std::move(highlighter)},
      failureHandled_{false}
{
    stream_ << std::boolalpha;
}

void ProgressBarConsoleReporter::testSuiteBegin(const TestSuiteMetadata& testSuiteMetadata)
{
    const auto stageName = "test";
    const auto resolution = testSuiteMetadata.testClassesToRun;
    const auto displayElapsedTime = true;

    progressBar_.emplace(stageName, resolution, progressBarPrinter_, displayElapsedTime);
}

void ProgressBarConsoleReporter::testSuiteEnd(const TestSuiteResult& testSuiteResult)
{
    progressBar_->updateSummary(
        testSuiteResult.testClassesFailed, testSuiteResult.testSuiteMetadata.testClassesToSkip,
        testSuiteResult.testClassesPassed, testSuiteResult.testMethodsPassed, testSuiteResult.assertionsPassed
    );

    progressBar_.reset();

    const auto output = stream_.str();

    if (!output.empty())
    {
        outputPrinter_(output);
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

using Highlighter = ProgressBarConsoleReporter::Highlighter;

TextHighlight getLevelHighlight(const Level level)
{
    switch (level)
    {
    case Level::none:
        return TextHighlight::None;
    case Level::debug:
        return TextHighlight::Blue;
    case Level::info:
        return TextHighlight::Green;
    case Level::warning:
        return TextHighlight::Yellow;
    case Level::error:
    case Level::critical:
        return TextHighlight::Red;
    }

    return TextHighlight::None;
}

std::string highlightLevel(const Highlighter& highlighter, const Level level)
{
    return highlighter(toString(level), getLevelHighlight(level));
}

void printLogs(
    std::ostringstream& stream, const Highlighter& highlighter, const std::vector<Log>& logs, size_t logLimit
)
{
    for (auto index = 0uz; index < std::min(logs.size(), logLimit); ++index)
    {
        stream << highlightLevel(highlighter, logs[index].level) << " " << logs[index].message << std::endl;
    }
}

std::string highlightTestClassMethod(const Highlighter& highlighter, const TestMethodMetadata& testMethodMetadata)
{
    return highlighter(
        testMethodMetadata.testClassMetadata.testClassName + "::" + testMethodMetadata.testMethodName,
        TextHighlight::Magenta
    );
}

void printTestMethodFailureHeader(
    std::ostringstream& stream, const Highlighter& highlighter, const TestMethodMetadata& testMethodMetadata
)
{
    stream << "  " << highlighter("Test method failed", TextHighlight::Red) << " "
           << testMethodMetadata.testClassMetadata.testClassFilePath << "(" << testMethodMetadata.testMethodLineNumber
           << ")" << std::endl
           << "    within " << highlightTestClassMethod(highlighter, testMethodMetadata) << std::endl;
}

void printException(std::ostringstream& stream, const std::string& heading, const ExceptionMetadata& exceptionMetadata)
{
    stream << "      " << heading << ":" << std::endl
           << "        " << exceptionMetadata.exceptionType << ": " << exceptionMetadata.exceptionMessage << std::endl;
}

const char* getExceptionStageQualifier(const TestMethodResult& testMethodResult)
{
    switch (testMethodResult.setUpStatus)
    {
    case StageStatus::NotRun:
        return " during construction";
    case StageStatus::Failed:
        return " during setup";
    case StageStatus::Succeeded:
        return "";
    }

    return "";
}

}

void ProgressBarConsoleReporter::testMethodEnd(const TestMethodResult& testMethodResult)
{
    const auto& testMethodMetadata = testMethodResult.testMethodMetadata;

    const auto& exceptionMetadata = testMethodResult.exceptionMetadata;

    const auto& cleanUpExceptionMetadata = testMethodResult.cleanUpExceptionMetadata;

    if (failureHandled_)
    {
        if (cleanUpExceptionMetadata)
        {
            printException(stream_, "an exception was also thrown during cleanup", *cleanUpExceptionMetadata);
        }
    }
    else if (exceptionMetadata)
    {
        printTestMethodFailureHeader(stream_, highlighter_, testMethodMetadata);

        printException(
            stream_, std::string{"an exception was thrown"} + getExceptionStageQualifier(testMethodResult),
            *exceptionMetadata
        );

        if (cleanUpExceptionMetadata)
        {
            printException(stream_, "an exception was also thrown during cleanup", *cleanUpExceptionMetadata);
        }
    }
    else if (cleanUpExceptionMetadata)
    {
        printTestMethodFailureHeader(stream_, highlighter_, testMethodMetadata);

        printException(stream_, "an exception was thrown during cleanup", *cleanUpExceptionMetadata);
    }
    else if (!testMethodResult.loggingSuccess)
    {
        printTestMethodFailureHeader(stream_, highlighter_, testMethodMetadata);

        stream_ << "      "
                << highlightLevel(
                       highlighter_, testMethodMetadata.testClassMetadata.testSuiteMetadata.loggingLevelFailure
                   );

        if (testMethodResult.failingLogs.size() > logLimit)
        {
            stream_ << "(s) or above were logged (displaying first " << logLimit << "):" << std::endl;
        }
        else
        {
            stream_ << "(s) or above were logged:" << std::endl;
        }

        printLogs(stream_, highlighter_, testMethodResult.failingLogs, logLimit);
    }
    else
    {
        return;
    }

    stream_ << std::endl;
}

void ProgressBarConsoleReporter::assertionEnd(const AssertionResult& assertionResult)
{
    if (assertionResult.assertionSuccess)
    {
        return;
    }

    if (failureHandled_)
    {
        stream_ << std::endl;
    }

    failureHandled_ = true;

    const auto& assertionMetadata = assertionResult.assertionMetadata;
    const auto& testClassMetadata = assertionMetadata.testMethodMetadata.testClassMetadata;

    stream_ << "  " << highlighter_("Assertion failed", TextHighlight::Red) << " "
            << testClassMetadata.testClassFilePath << "(" << assertionMetadata.assertionLineNumber << ")" << std::endl
            << "    within " << highlightTestClassMethod(highlighter_, assertionMetadata.testMethodMetadata)
            << std::endl;

    if (assertionResult.exceptionMetadata)
    {
        printException(stream_, "an exception was thrown", *assertionResult.exceptionMetadata);

        return;
    }

    const auto visitor = [&](auto&& argument)
    {
        using ArgumentType = std::decay_t<decltype(argument)>;

        if constexpr (std::is_same_v<ArgumentType, UnaryAssertion>)
        {
            stream_ << "      " << highlighter_("REQUIRE", TextHighlight::Blue) << "(" << assertionMetadata.expression
                    << ')' << std::endl;

            const auto evaluationIsNotRedundant =
                assertionMetadata.expression != "true" && assertionMetadata.expression != "false";

            if (evaluationIsNotRedundant)
            {
                stream_ << "        evaluates to " << assertionResult.assertionSuccess << std::endl;
            }
        }
        else if constexpr (std::is_same_v<ArgumentType, BinaryAssertion>)
        {
            stream_ << "      " << highlighter_("REQUIRE", TextHighlight::Blue) << "(" << assertionMetadata.expression
                    << ')' << std::endl;

            const auto representation =
                argument.firstRepresentation + ' ' + argument.operation + ' ' + argument.secondRepresentation;

            const auto representationIsNotRedundant = assertionMetadata.expression != representation;

            if (representationIsNotRedundant)
            {
                stream_ << "              " << representation << std::endl;
            }

            stream_ << "        evaluates to " << assertionResult.assertionSuccess << std::endl;
        }
        else if constexpr (std::is_same_v<ArgumentType, ThrowAssertion>)
        {
            stream_ << "      " << highlighter_("REQUIRE_THROW", TextHighlight::Blue) << "("
                    << assertionMetadata.expression << ')' << std::endl;

            if (!argument.exceptionThrown)
            {
                stream_ << "        no exception was thrown" << std::endl;
            }
            else
            {
                stream_ << "        a different type of exception was thrown:\n"
                        << "          " << argument.actualException << ": " << argument.exceptionMessage << std::endl;
            }
        }
        else if constexpr (std::is_same_v<ArgumentType, LogAssertion>)
        {
            stream_ << "      " << highlighter_("REQUIRE_LOGGING", TextHighlight::Blue) << "("
                    << assertionMetadata.expression << ')' << std::endl;

            if (argument.expectedLogs.size() > logLimit)
            {
                stream_ << "        mismatch between expected logs (displaying first " << logLimit << "):" << std::endl
                        << std::endl;
            }
            else
            {
                stream_ << "        mismatch between expected logs:" << std::endl << std::endl;
            }

            printLogs(stream_, highlighter_, argument.expectedLogs, logLimit);

            if (argument.actualLogs.size() > logLimit)
            {
                stream_ << std::endl
                        << "        and actual logs (displaying first " << logLimit << "):" << std::endl
                        << std::endl;
            }
            else
            {
                stream_ << std::endl << "        and actual logs:" << std::endl << std::endl;
            }

            printLogs(stream_, highlighter_, argument.actualLogs, logLimit);
        }
        else
        {
            static_assert(false, "Assertion visitor is non-exhaustive");
        }
    };

    std::visit(visitor, assertionResult.assertion);
}

}
