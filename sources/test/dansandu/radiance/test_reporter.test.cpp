#include "dansandu/radiance/test_reporter.test.hpp"
#include "dansandu/radiance/utility.hpp"

#include <filesystem>
#include <ios>
#include <ranges>
#include <string_view>
#include <type_traits>

using dansandu::radiance::utility::join;

namespace dansandu::radiance::test_reporter
{

TestReporter::TestReporter()
{
    stream_ << std::boolalpha;
}

void TestReporter::testSuiteBegin(const TestSuiteMetadata& metadata)
{
    stream_ << "Begin test suite" << std::endl << "  test cases being run: " << metadata.testCasesBeingRun << std::endl;
}

void TestReporter::testSuiteEnd(const TestSuiteResult& result)
{
    stream_ << "End test suite" << std::endl
            << "  test cases ran: " << result.testCasesRan << std::endl
            << "  test cases passed: " << result.testCasesPassed << std::endl
            << "  test cases failed: " << result.testCasesFailed << std::endl
            << "  test assertions ran: " << result.assertionsRan << std::endl
            << "  test assertions passed: " << result.assertionsPassed << std::endl
            << "  test assertions failed: " << result.assertionsFailed << std::endl
            << "  test suite success: " << result.testSuiteSuccess << std::endl;
}

void TestReporter::testCaseBegin(const TestCaseMetadata& metadata)
{
    const auto fileName = std::filesystem::path{metadata.filePath}.filename();
    stream_ << "  Begin test case" << std::endl
            << "    test case name: \"" << metadata.testCaseName << '"' << std::endl
            << "    file: " << fileName << std::endl
            << "    line: " << metadata.lineNumber << std::endl;
}

void TestReporter::testCaseEnd(const TestCaseResult& result)
{
    stream_ << "  End test case" << std::endl
            << "    test assertions ran: " << result.assertionsRan << std::endl
            << "    test assertions passed: " << result.assertionsPassed << std::endl
            << "    test assertions failed: " << result.assertionsFailed << std::endl
            << "    test case success: " << result.testCaseSuccess << std::endl;
}

void TestReporter::testCaseRunBegin(const TestCaseRunMetadata& metadata)
{
    stream_ << "    Begin test case run" << std::endl;
}

void TestReporter::testCaseRunEnd(const TestCaseRunResult& result)
{
    stream_ << "    End test case run" << std::endl
            << "      test assertions ran: " << result.assertionsRan << std::endl
            << "      test assertions passed: " << result.assertionsPassed << std::endl
            << "      test assertions failed: " << result.assertionsFailed << std::endl
            << "      test case run success: " << result.testCaseRunSuccess << std::endl;

    if (result.exceptionMetadata)
    {
        stream_ << "      exception type: " << result.exceptionMetadata->exceptionType << std::endl
                << "      exception message: \"" << result.exceptionMetadata->exceptionMessage << '"' << std::endl;
    }
}

void TestReporter::sectionBegin(const SectionMetadata& metadata)
{
    stream_ << "      Begin section" << std::endl << "        sections: " << join(metadata.sections) << std::endl;
}

void TestReporter::sectionEnd(const SectionResult& result)
{
    stream_ << "      End section" << std::endl
            << "        sections: " << join(result.sectionMetadata.sections) << std::endl
            << "        section success: " << result.sectionSuccess << std::endl;
}

void TestReporter::assertionBegin(const AssertionMetadata& metadata)
{
    stream_ << "      Begin assertion" << std::endl
            << "        expression: " << metadata.expression << std::endl
            << "        line: " << metadata.lineNumber << std::endl;
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
                            << "          operation: " << argument.operation << std::endl;
                }
                else if constexpr (std::is_same_v<ArgumentType, ThrowAssertion>)
                {
                    stream_ << "        Throw assertion" << std::endl
                            << "          expected exception: " << argument.expectedException << std::endl
                            << "          exception thrown: " << argument.exceptionThrown << std::endl
                            << "          exception message: " << argument.exceptionMessage << std::endl;
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
        stream_ << "        exception type: " << result.exceptionMetadata->exceptionType << std::endl
                << "        exception message: \"" << result.exceptionMetadata->exceptionMessage << '"' << std::endl;
    }
}

std::wstring TestReporter::getString() const
{
    return stream_.str();
}

}
