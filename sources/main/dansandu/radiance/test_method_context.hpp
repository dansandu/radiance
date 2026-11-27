#pragma once

#include "dansandu/journey/reporter.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"

#include <functional>
#include <string>
#include <typeinfo>
#include <vector>

namespace dansandu::radiance::test_method_context
{

class PRALINE_EXPORT TestMethodContext
{
public:
    TestMethodContext(const TestMethodContext&) = delete;
    TestMethodContext(TestMethodContext&&) noexcept = delete;
    TestMethodContext& operator=(const TestMethodContext&) = delete;
    TestMethodContext& operator=(TestMethodContext&&) noexcept = delete;

    TestMethodContext(
        TestMethodResult& testMethodResult, const dansandu::journey::reporter::InMemoryReporter& inMemoryReporter,
        dansandu::radiance::reporter::IReporter& reporter
    );

    ~TestMethodContext() noexcept;

    void handleAssertion(
        const char* const expressionString, const int lineNumber,
        const std::function<void(AssertionResult&)>& expression
    );

    void handleLoggingAssertion(
        const char* const expressionString, const int lineNumber, const std::vector<Log>& expectedLogs,
        const std::function<void()>& expression
    );

    void handleThrowAssertion(
        const char* const expressionString, const int lineNumber, const std::function<void()>& expression,
        const std::type_info& expectedExceptionType
    );

private:
    TestMethodResult& testMethodResult_;
    const dansandu::journey::reporter::InMemoryReporter& inMemoryReporter_;
    dansandu::radiance::reporter::IReporter& reporter_;
};

}
