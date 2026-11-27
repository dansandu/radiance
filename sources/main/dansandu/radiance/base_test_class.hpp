#pragma once

#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/test_method_context.hpp"

#include <functional>
#include <typeinfo>
#include <vector>

namespace dansandu::radiance::test_class_descriptor
{

class TestClassDescriptor;

}

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

    virtual void setUp();

    virtual void cleanUp();

protected:
    // The assertion handlers are used by the assertion macros within test classes.
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
    friend class dansandu::radiance::test_class_descriptor::TestClassDescriptor;

    void setTestMethodContext(dansandu::radiance::test_method_context::TestMethodContext& testMethodContext);

    dansandu::radiance::test_method_context::TestMethodContext* testMethodContext_;
};

}
