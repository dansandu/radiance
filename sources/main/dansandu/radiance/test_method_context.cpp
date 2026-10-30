#include "dansandu/radiance/test_method_context.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"

#include <functional>
#include <string>
#include <vector>

using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::assertion::Assertion;

namespace dansandu::radiance::test_method_context
{

TestMethodContext::TestMethodContext(TestMethodResult& testMethodResult, IReporter& reporter)
 : testMethodResult_{testMethodResult}, reporter_{reporter}
{
}

TestMethodContext::~TestMethodContext() noexcept
{
}

void TestMethodContext::handleAssertion(const char* const expressionString, const int lineNumber,
                               const std::function<void(AssertionResult&)>& expression)
{
    ++testMethodResult_.assertionsRan;

    const auto assertionMetadata = AssertionMetadata{.expression = expressionString,
                                                     .assertionLineNumber = lineNumber};

    auto assertion = Assertion{assertionMetadata, reporter_};

    try
    {
        assertion.invoke(expression);

        ++testMethodResult_.assertionsPassed;
    }
    catch (...)
    {
        ++testMethodResult_.assertionsFailed;

        throw;
    }
}

void TestMethodContext::handleLoggingAssertion(const char* const expressionString, const int lineNumber,
                                  const std::vector<Log>& expectedLogs, const std::function<void()>& expression)
{
    ++testMethodResult_.assertionsRan;

    const auto assertionMetadata = AssertionMetadata{.expression = expressionString,
                                                     .assertionLineNumber = lineNumber};

    auto assertion = Assertion{assertionMetadata, reporter_};

    try
    {
        assertion.logInvoke(inMemoryReporter_, expectedLogs, expression);

        ++testMethodResult_.assertionsPassed;
    }
    catch (...)
    {
        ++testMethodResult_.assertionsFailed;

        throw;
    }
}

void TestMethodContext::handleThrowAssertion(
    const char* const expressionString, const int lineNumber,
    const std::function<void()>& expression, const std::type_info& expectedExceptionType)
{
    ++testMethodResult_.assertionsRan;

    const auto assertionMetadata = AssertionMetadata{
        .testMethodMetadata = testMethodResult_.testMethodMetadata,
        .expression = expressionString,
        .assertionLineNumber = lineNumber,
    };

    auto assertion = Assertion{assertionMetadata, reporter_};

    try
    {
        assertion.throwInvoke(expression, expectedExceptionType);

        ++testMethodResult_.assertionsPassed;
    }
    catch (...)
    {
        ++testMethodResult_.assertionsFailed;

        throw;
    }
}

}
