#include "dansandu/radiance/test_reporter.hpp"
#include "dansandu/journey/common.hpp"
#include "dansandu/radiance/utility.hpp"

#include <filesystem>
#include <ios>
#include <ranges>
#include <string_view>
#include <type_traits>

using dansandu::journey::toString;
using dansandu::radiance::utility::join;

namespace dansandu::radiance::test_reporter
{

TestReporter::TestReporter()
{
    stream_ << std::boolalpha;
}

void TestReporter::testSuiteBegin(const TestSuiteMetadata& metadata)
{
    stream_ << "Begin test suite" << std::endl
            << "  test classes to run: " << metadata.testClassesToRun << std::endl
            << "  logging level failure: " << toString(metadata.loggingLevelFailure) << std::endl;
}

void TestReporter::testSuiteEnd(const TestSuiteResult& result)
{
    stream_ << "End test suite" << std::endl
            << "  test classes ran: " << result.testClassesRan << std::endl
            << "  test classes passed: " << result.testClassesPassed << std::endl
            << "  test classes failed: " << result.testClassesFailed << std::endl
            << "  test methods ran: " << result.testMethodsRan << std::endl
            << "  test methods passed: " << result.testMethodsPassed << std::endl
            << "  test methods failed: " << result.testMethodsFailed << std::endl
            << "  test assertions ran: " << result.assertionsRan << std::endl
            << "  test assertions passed: " << result.assertionsPassed << std::endl
            << "  test assertions failed: " << result.assertionsFailed << std::endl
            << "  logging success: " << result.loggingSuccess << std::endl
            << "  test suite success: " << result.testSuiteSuccess << std::endl;
}

void TestReporter::testClassBegin(const TestClassMetadata& metadata)
{
    const auto fileName = std::filesystem::path{metadata.testClassFilePath}.filename();
    stream_ << "  Begin test class" << std::endl
            << "    test class name: " << metadata.testClassName << std::endl
            << "    test class file name: " << fileName << std::endl
            << "    test class line number: " << metadata.testClassLineNumber << std::endl;
}

void TestReporter::testClassEnd(const TestClassResult& result)
{
    stream_ << "  End test class" << std::endl
            << "    test methods ran: " << result.testMethodsRan << std::endl
            << "    test methods passed: " << result.testMethodsPassed << std::endl
            << "    test methods failed: " << result.testMethodsFailed << std::endl
            << "    test assertions ran: " << result.assertionsRan << std::endl
            << "    test assertions passed: " << result.assertionsPassed << std::endl
            << "    test assertions failed: " << result.assertionsFailed << std::endl
            << "    logging success: " << result.loggingSuccess << std::endl
            << "    test class success: " << result.testClassSuccess << std::endl;
}

void TestReporter::testMethodBegin(const TestMethodMetadata& metadata)
{
    stream_ << "    Begin test method" << std::endl
            << "      test method name: " << metadata.testMethodName << std::endl
            << "      test method line number: " << metadata.testMethodLineNumber << std::endl;
}

void TestReporter::testMethodEnd(const TestMethodResult& result)
{
    stream_ << "    End test method" << std::endl
            << "      test assertions ran: " << result.assertionsRan << std::endl
            << "      test assertions passed: " << result.assertionsPassed << std::endl
            << "      test assertions failed: " << result.assertionsFailed << std::endl
            << "      test failing logs:" << std::endl;

    for (const auto& log : result.failingLogs)
    {
        stream_ << "        " << toString(log.level) << " " << log.message << std::endl;
    }

    stream_ << "      logging success: " << result.loggingSuccess << std::endl
            << "      setup status: " << toString(result.setUpStatus) << std::endl
            << "      cleanup status: " << toString(result.cleanUpStatus) << std::endl
            << "      test method success: " << result.testMethodSuccess << std::endl;

    if (result.exceptionMetadata)
    {
        stream_ << "      exception type: " << result.exceptionMetadata->exceptionType << std::endl
                << "      exception message: " << result.exceptionMetadata->exceptionMessage << std::endl;
    }

    if (result.cleanUpExceptionMetadata)
    {
        stream_ << "      cleanup exception type: " << result.cleanUpExceptionMetadata->exceptionType << std::endl
                << "      cleanup exception message: " << result.cleanUpExceptionMetadata->exceptionMessage
                << std::endl;
    }
}

void TestReporter::assertionBegin(const AssertionMetadata& metadata)
{
    stream_ << "      Begin assertion" << std::endl
            << "        expression: " << metadata.expression.c_str() << std::endl
            << "        line: " << metadata.assertionLineNumber << std::endl;
}

void TestReporter::assertionEnd(const AssertionResult& result)
{
    stream_ << "      End assertion" << std::endl
            << "        assertion success: " << result.assertionSuccess << std::endl;

    if (!result.exceptionMetadata)
    {
        std::visit(
            [&](auto&& argument)
            {
                using ArgumentType = std::decay_t<decltype(argument)>;

                if constexpr (std::is_same_v<ArgumentType, UnaryAssertion>)
                {
                    stream_ << "        Unary assertion" << std::endl
                            << "          representation: " << argument.representation << std::endl;
                }
                else if constexpr (std::is_same_v<ArgumentType, BinaryAssertion>)
                {
                    stream_ << "        Binary assertion" << std::endl
                            << "          first representation: " << argument.firstRepresentation << std::endl
                            << "          second representation: " << argument.secondRepresentation << std::endl
                            << "          operation: " << argument.operation.c_str() << std::endl;
                }
                else if constexpr (std::is_same_v<ArgumentType, ThrowAssertion>)
                {
                    stream_ << "        Throw assertion" << std::endl
                            << "          expected exception: " << argument.expectedException.c_str() << std::endl
                            << "          actual exception: " << argument.actualException.c_str() << std::endl
                            << "          exception thrown: " << argument.exceptionThrown << std::endl;
                }
                else if constexpr (std::is_same_v<ArgumentType, LogAssertion>)
                {
                    stream_ << "        Log assertion" << std::endl << "          expected logs:" << std::endl;

                    for (const auto& log : argument.expectedLogs)
                    {
                        stream_ << "            " << toString(log.level) << " " << log.message << std::endl;
                    }

                    stream_ << "          actual logs:" << std::endl;

                    for (const auto& log : argument.actualLogs)
                    {
                        stream_ << "            " << toString(log.level) << " " << log.message << std::endl;
                    }
                }
                else
                {
                    static_assert(false, "Assertion visitor is non-exhaustive");
                }
            },
            result.assertion
        );
    }
    else
    {
        stream_ << "        exception type: " << result.exceptionMetadata->exceptionType << std::endl;
    }
}

std::string TestReporter::getString() const
{
    return stream_.str();
}

}
