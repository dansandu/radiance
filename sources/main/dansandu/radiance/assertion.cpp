#include "dansandu/radiance/assertion.hpp"
#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/reporter.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/binding.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/utility.hpp"

#include <algorithm>
#include <cstdint>
#include <string>
#include <typeinfo>

using dansandu::journey::Level;
using dansandu::journey::logging::Logger;
using dansandu::journey::reporter::InMemoryReporter;
using dansandu::journey::utility::getTypeName;
using dansandu::radiance::exception::AssertionFailureException;
using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::utility::journeyLogsToRadianceLogs;

namespace dansandu::radiance::assertion
{

namespace
{

template<typename... A, typename... AA>
void wrapInTryCatchAndInvoke(AssertionResult& assertionResult, const std::function<void(A...)>& expression,
                             AA&&... arguments)
{
    try
    {
        expression(std::forward<AA>(arguments)...);
    }
    catch (const std::exception& exception)
    {
        const auto includeNamespace = true;

        assertionResult.exceptionMetadata = ExceptionMetadata{
            .exceptionType = getTypeName(includeNamespace, exception),
            .exceptionMessage = exception.what(),
        };

        throw;
    }
    catch (...)
    {
        assertionResult.exceptionMetadata = ExceptionMetadata{
            .exceptionType = "<Unknown>",
            .exceptionMessage = "<Unknown>",
        };

        throw;
    }
}

}

Assertion::Assertion(const AssertionMetadata& assertionMetadata, IReporter& reporter)
    : assertionResult_{
          .assertionMetadata = assertionMetadata,
          .assertionSuccess = false,
          .assertion = {},
          .exceptionMetadata = {},
      },
      reporter_{reporter}
{
    reporter_.assertionBegin(assertionResult_.assertionMetadata);
}

Assertion::~Assertion() noexcept
{
    reporter_.assertionEnd(assertionResult_);
}

void Assertion::invoke(const std::function<void(AssertionResult&)>& expression)
{
    wrapInTryCatchAndInvoke(assertionResult_, expression, assertionResult_);

    if (!assertionResult_.assertionSuccess)
    {
        THROW(AssertionFailureException, "Assertion failed");
    }
}

void Assertion::logInvoke(const InMemoryReporter& testCaseInMemoryReporter, const std::vector<Log>& expectedLogs,
                          const std::function<void()>& expression)
{
    static uint64_t uniqueId = 0;

    const auto reporterName = "RadianceAssertionLogTracker_" + std::to_string(uniqueId++);

    const auto reporter = InMemoryReporter{};

    Logger::getGlobalInstance().addReporter(reporterName, Level::debug, reporter);

    testCaseInMemoryReporter.enable(false);

    wrapInTryCatchAndInvoke(assertionResult_, expression);

    testCaseInMemoryReporter.enable(true);

    Logger::getGlobalInstance().removeReporter(reporterName);

    auto actualLogs = journeyLogsToRadianceLogs(reporter.getLoggedEntries());

    assertionResult_.assertion = LogAssertion{
        .expectedLogs = expectedLogs,
        .actualLogs = actualLogs,
    };

    assertionResult_.assertionSuccess =
        std::equal(expectedLogs.cbegin(), expectedLogs.cend(), actualLogs.cbegin(), actualLogs.cend());

    if (!assertionResult_.assertionSuccess)
    {
        THROW(AssertionFailureException, "Assertion failed");
    }
}

void Assertion::throwInvoke(const std::function<void()>& expression, const std::type_info& expectedExceptionType)
{
    const auto includeNamespace = true;

    try
    {
        expression();
    }
    catch (const std::exception& exception)
    {
        assertionResult_.assertion = ThrowAssertion{
            .exceptionMessage = exception.what(),
            .expectedException = getTypeName(includeNamespace, expectedExceptionType),
            .actualException = getTypeName(includeNamespace, exception),
            .exceptionThrown = true,
        };

        if (typeid(exception) == expectedExceptionType)
        {
            assertionResult_.assertionSuccess = true;
        }
        else
        {
            throw;
        }
    }
    catch (...)
    {
        assertionResult_.assertion = ThrowAssertion{
            .exceptionMessage = "<Unknown>",
            .expectedException = getTypeName(includeNamespace, expectedExceptionType),
            .actualException = "<Unknown>",
            .exceptionThrown = true,
        };

        throw;
    }

    if (!assertionResult_.assertionSuccess)
    {
        assertionResult_.assertion = ThrowAssertion{
            .exceptionMessage = {},
            .expectedException = getTypeName(includeNamespace, expectedExceptionType),
            .actualException = {},
            .exceptionThrown = false,
        };

        THROW(AssertionFailureException, "Assertion failed");
    }
}

}
