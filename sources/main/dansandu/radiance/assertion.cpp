#include "dansandu/radiance/assertion.hpp"
#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/logging.hpp"
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

using dansandu::journey::Level;
using dansandu::journey::exception::WideException;
using dansandu::journey::logging::Logger;
using dansandu::journey::reporter::InMemoryReporter;
using dansandu::journey::utility::toWideString;
using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::utility::getExceptionTypeName;
using dansandu::radiance::utility::journeyLogsToRadianceLogs;

namespace dansandu::radiance::assertion
{

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
    catch (const WideException& wideException)
    {
        assertionResult.exceptionMetadata = ExceptionMetadata{
            .exceptionType = toWideString(getExceptionTypeName(wideException)),
            .exceptionMessage = wideException.getMessage(),
            .sectionsCallStack = {},
        };

        throw;
    }
    catch (const std::exception& exception)
    {
        assertionResult.exceptionMetadata = ExceptionMetadata{
            .exceptionType = toWideString(getExceptionTypeName(exception)),
            .exceptionMessage = toWideString(exception.what()),
            .sectionsCallStack = {},
        };

        throw;
    }
    catch (...)
    {
        assertionResult.exceptionMetadata = ExceptionMetadata{
            .exceptionType = L"Unknown",
            .exceptionMessage = L"Unknown",
            .sectionsCallStack = {},
        };

        throw;
    }
}

}

void Assertion::invoke(const std::function<void(AssertionResult&)>& expression)
{
    wrapInTryCatchAndInvoke(assertionResult_, expression, assertionResult_);

    if (!assertionResult_.assertionSuccess)
    {
        THROW(std::runtime_error, "Assertion failed");
    }
}

void Assertion::logInvoke(const InMemoryReporter& testCaseInMemoryReporter, const std::vector<Log>& expectedLogs,
                          const std::function<void()>& expression)
{
    static uint64_t uniqueId = 0;

    const auto reporterName = L"RadianceAssertionLogTracker_" + std::to_wstring(uniqueId++);

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
        THROW(std::runtime_error, "Assertion failed");
    }
}

}
