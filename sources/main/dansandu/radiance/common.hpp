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
    int testClassesTotal = 0;
    int testClassesToRun = 0;
    int testClassesToSkip = 0;
    dansandu::journey::Level loggingLevelFailure = dansandu::journey::Level::warning;
};

struct ExceptionMetadata
{
    std::string exceptionType;
    std::string exceptionMessage;
};

struct TestSuiteResult
{
    TestSuiteMetadata testSuiteMetadata;
    int testClassesRan = 0;
    int testClassesPassed = 0;
    int testClassesFailed = 0;
    int assertionsRan = 0;
    int assertionsPassed = 0;
    int assertionsFailed = 0;
    bool loggingSuccess = false;
    bool testSuiteSuccess = false;
};

struct TestClassMetadata
{
    TestSuiteMetadata testSuiteMetadata;
    std::string testClassName;
    std::string testClassFilePath;
    int testClassLineNumber = 0;
};

struct TestClassResult
{
    TestClassMetadata testClassMetadata;
    int assertionsRan = 0;
    int assertionsPassed = 0;
    int assertionsFailed = 0;
    bool loggingSuccess = false;
    bool testClassSuccess = false;
};

struct TestMethodMetadata
{
    TestClassMetadata testClassMetadata;
    std::string testMethodName;
    int testMethodLineNumber = 0;
};

struct Log
{
    friend bool operator==(const Log& left, const Log& right) = default;

    dansandu::journey::Level level;
    std::string message;
};

struct TestMethodResult
{
    TestMethodMetadata testMethodMetadata;
    int assertionsRan = 0;
    int assertionsPassed = 0;
    int assertionsFailed = 0;
    std::vector<Log> failingLogs;
    bool loggingSuccess = false;
    bool testMethodSuccess = false;
    std::optional<ExceptionMetadata> exceptionMetadata;
};

struct AssertionMetadata
{
    TestMethodMetadata testMethodMetadata;
    std::string expression;
    int assertionLineNumber = 0;
};

struct UnaryAssertion
{
    std::string representation;
};

struct BinaryAssertion
{
    std::string firstRepresentation;
    std::string secondRepresentation;
    std::string operation;
};

struct ThrowAssertion
{
    std::string exceptionMessage;
    std::string expectedException;
    std::string actualException;
    bool exceptionThrown = false;
};

struct LogAssertion
{
    std::vector<Log> expectedLogs;
    std::vector<Log> actualLogs;
};

struct AssertionResult
{
    AssertionMetadata assertionMetadata;
    bool assertionSuccess = false;
    std::variant<UnaryAssertion, BinaryAssertion, ThrowAssertion, LogAssertion> assertion;
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
