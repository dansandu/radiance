#include "dansandu/radiance/base_test_class.hpp"
#include "dansandu/journey/exception.hpp"

using dansandu::radiance::test_method_context::TestMethodContext;

namespace dansandu::radiance::base_test_class
{

BaseTestClass() : testMethodContext_{nullptr}
{
}

BaseTestClass::~BaseTestClass() noexcept
{

}

void BaseTestClass::setTestMethodContext(TestMethodContext* const testMethodContext)
{
    testMethodContext_ = testMethodContext;
}

void BaseTestClass::handleAssertion(
    const char* const expressionString, const int lineNumber,
    const std::function<void(AssertionResult&)>& expression)
{
    if (testMethodContext_ == nullptr)
    {
        THROW(std::logic_error, "TestMethodContext was not set");
    }

    testMethodContext_->handleAssertion(expressionString, lineNumber, expression);
}

void BaseTestClass::handleLoggingAssertion(const char* const expressionString, const int lineNumber,
                                           const std::vector<Log>& expectedLogs, const std::function<void()>& expression)
{
    if (testMethodContext_ == nullptr)
    {
        THROW(std::logic_error, "TestMethodContext was not set");
    }

    testMethodContext_->handleLoggingAssertion(expressionString, lineNumber, expectedLogs, expression);
}

void BaseTestClass::handleThrowAssertion(const char* const expressionString, const int lineNumber,
                                         const std::function<void()>& expression, const std::type_info& expectedExceptionType)
{
    if (testMethodContext_ == nullptr)
    {
        THROW(std::logic_error, "TestMethodContext was not set");
    }

    testMethodContext_->handleThrowAssertion(expressionString, lineNumber, expectedLogs, expectedExceptionType);
}

void BaseTestClass::setUp()
{
}

void BaseTestClass::cleanUp()
{
}

}
