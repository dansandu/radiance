#pragma once

#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/test_method_context.hpp"

#include <functional>
#include <vector>
#include <typeinfo>

namespace dansandu::radiance::base_test_class
{

class PRALINE_EXPORT BaseTestClass
{
public:
    BaseTestClass(const BaseTestClass&) = delete;
    BaseTestClass(BaseTestClass&&) noexcept = delete;
    BaseTestClass& operator=(const BaseTestClass&) = delete;
    BaseTestClass& operator=(BaseTestClass&&) noexcept = delete;

    BaseTestClass();

    virtual ~BaseTestClass() noexcept = 0;

    void setTestMethodContext(dansandu::radiance::test_method_context::TestMethodContext* const testMethodContext);

    void handleAssertion(const char* const expressionString, const int lineNumber,
                         const std::function<void(AssertionResult&)>& expression);

    void handleLoggingAssertion(const char* const expressionString, const int lineNumber,
                                const std::vector<Log>& expectedLogs, const std::function<void()>& expression);

    void handleThrowAssertion(const char* const expressionString, const int lineNumber,
                              const std::function<void()>& expression, const std::type_info& expectedExceptionType);

    virtual void setUp();

    virtual void cleanUp();

private:
    dansandu::radiance::test_method_context::TestMethodContext* const testMethodContext_;
};

}
