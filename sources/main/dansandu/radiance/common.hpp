#pragma once

#include "dansandu/journey/common.hpp"

#include <cmath>
#include <optional>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

namespace dansandu::radiance
{

struct TestSuiteMetadata
{
    int testCasesTotal = 0;
    int testCasesBeingRun = 0;
    int testCasesBeingSkipped = 0;
    dansandu::journey::Level loggingLevelFailure = dansandu::journey::Level::warning;
};

struct ExceptionMetadata
{
    std::wstring exceptionType;
    std::wstring exceptionMessage;
    std::vector<std::wstring> sectionsCallStack;
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
    bool loggingSuccess = false;
    bool testSuiteSuccess = false;
};

struct TestCaseMetadata
{
    TestSuiteMetadata testSuiteMetadata;
    std::wstring testCaseName;
    std::string filePath;
    int lineNumber = 0;
};

struct TestCaseResult
{
    TestCaseMetadata testCaseMetadata;
    int assertionsRan = 0;
    int assertionsPassed = 0;
    int assertionsFailed = 0;
    bool loggingSuccess = false;
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
    bool loggingSuccess = false;
    bool testCaseRunSuccess = false;
    std::optional<ExceptionMetadata> exceptionMetadata;
};

struct SectionMetadata
{
    TestCaseRunMetadata testCaseRunMetadata;
    std::vector<std::wstring> sections;
};

struct SectionResult
{
    SectionMetadata sectionMetadata;
    bool exceptionThrown = false;
    bool loggingSuccess = false;
    bool sectionSuccess = false;
};

struct AssertionMetadata
{
    SectionMetadata sectionMetadata;
    std::string expression;
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
    std::string operation;
};

struct ThrowAssertion
{
    std::wstring exceptionMessage;
    std::string expectedException;
    std::string actualException;
    bool exceptionThrown = false;
};

struct AssertionResult
{
    AssertionMetadata assertionMetadata;
    bool assertionSuccess = false;
    std::variant<UnaryAssertion, BinaryAssertion, ThrowAssertion> assertion;
    std::optional<ExceptionMetadata> exceptionMetadata;
};

template<typename T>
struct Tolerance
{
    template<typename A>
    Tolerance(A&& target, const double relative = 1.0e-6, const double absolute = 1.0e-6)
        : target{std::forward<A>(target)}, relative{relative}, absolute{absolute}
    {
    }

    T target;
    double relative;
    double absolute;
};

template<typename A>
Tolerance(A&& target, const double relative = 1.0e-6, const double absolute = 1.0e-6) -> Tolerance<std::decay_t<A>>;

template<typename T, typename U>
bool operator==(const T& value, const Tolerance<U>& tolerance)
{
    using std::abs;
    return abs(value - tolerance.target) <= (tolerance.absolute + tolerance.relative * abs(tolerance.target));
}

template<typename T, typename U>
bool operator==(const Tolerance<T>& tolerance, const U& value)
{
    return value == tolerance;
}

template<typename T, typename U>
bool operator!=(const T& value, const Tolerance<U>& tolerance)
{
    using std::abs;
    return abs(value - tolerance.target) > (tolerance.absolute + tolerance.relative * abs(tolerance.target));
}

template<typename T, typename U>
bool operator!=(const Tolerance<T>& tolerance, const U& value)
{
    return value != tolerance;
}

}
