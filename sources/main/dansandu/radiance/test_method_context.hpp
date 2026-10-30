#pragma once

#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"

#include <functional>
#include <string>
#include <vector>
#include <typeinfo>

namespace dansandu::radiance::test_method_context
{

class PRALINE_EXPORT TestMethodContext
{
public:
    TestMethodContext(const TestMethodContext&) = delete;
    TestMethodContext(TestMethodContext&&) noexcept = delete;
    TestMethodContext& operator=(const TestMethodContext&) = delete;
    TestMethodContext& operator=(TestMethodContext&&) noexcept = delete;

    TestMethodContext(TestMethodResult& testMethodResult, dansandu::radiance::reporter::IReporter& reporter);

    ~TestMethodContext() noexcept;

    void handleAssertion(const char* const expressionString, const int lineNumber,
                         const std::function<void(AssertionResult&)>& expression);

    void handleLoggingAssertion(const char* const expressionString, const int lineNumber,
                                const std::vector<Log>& expectedLogs, const std::function<void()>& expression);

    void handleThrowAssertion(const char* const expressionString, const int lineNumber,
                              const std::function<void()>& expression, const std::type_info& expectedExceptionType);

private:
    TestMethodResult& testMethodResult_;
    dansandu::radiance::reporter::IReporter& reporter_;
};

}
