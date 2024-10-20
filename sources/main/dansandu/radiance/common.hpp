#pragma once

#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace dansandu::radiance
{

struct TestSuiteMetadata
{
    int testCasesTotal = 0;
    int testCasesBeingRun = 0;
    int testCasesBeingSkipped = 0;
};

struct ExceptionMetadata
{
    std::wstring exceptionType;
    std::wstring exceptionMessage;
};

struct TestSuiteResult
{
    TestSuiteMetadata testSuiteMetadata;
    int testCasesRan = 0;
    int testCasesPassed = 0;
    int testCasesFailed = 0;
    int assertionsRan = 0;
    int assertionsPassed = 0;
    int assertionsFailed = 0;
    bool testSuiteSuccess = false;
};

struct TestCaseMetadata
{
    TestSuiteMetadata testSuiteMetadata;
    std::wstring testCaseName;
    const char* filePath = nullptr;
    int lineNumber = 0;
};

struct TestCaseResult
{
    TestCaseMetadata testCaseMetadata;
    int assertionsRan = 0;
    int assertionsPassed = 0;
    int assertionsFailed = 0;
    bool testCaseSuccess = false;
};

struct TestCaseRunMetadata
{
    TestCaseMetadata testCaseMetadata;
};

struct TestCaseRunResult
{
    TestCaseRunMetadata testCaseRunMetadata;
    int assertionsRan = 0;
    int assertionsPassed = 0;
    int assertionsFailed = 0;
    bool testCaseRunSuccess = false;
    std::optional<ExceptionMetadata> exceptionMetadata;
};

struct SectionMetadata
{
    TestCaseRunMetadata testCaseRunMetadata;
    std::vector<const wchar_t*> sections;
};

struct SectionResult
{
    SectionMetadata sectionMetadata;
    bool sectionSuccess = false;
};

struct AssertionMetadata
{
    SectionMetadata sectionMetadata;
    const char* expression = nullptr;
    int lineNumber = 0;
};

struct UnaryAssertion
{
    std::wstring representation;
};

struct BinaryAssertion
{
    std::wstring firstRepresentation;
    std::wstring secondRepresentation;
    const char* operation = nullptr;
};

struct AssertionResult
{
    AssertionMetadata assertionMetadata;
    bool assertionSuccess = false;
    std::variant<UnaryAssertion, BinaryAssertion> assertion;
    std::optional<ExceptionMetadata> exceptionMetadata;
};

}
