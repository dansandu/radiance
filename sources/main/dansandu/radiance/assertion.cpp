#include "dansandu/radiance/assertion.hpp"
#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/binding.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/utility.hpp"

using dansandu::journey::exception::WideException;
using dansandu::journey::utility::toWideString;
using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::utility::getExceptionTypeName;

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

void Assertion::invoke(const std::function<void(AssertionResult&)>& expression)
{
    try
    {
        expression(assertionResult_);
    }
    catch (const WideException& wideException)
    {
        assertionResult_.exceptionMetadata = ExceptionMetadata{
            .exceptionType = toWideString(getExceptionTypeName(wideException)),
            .exceptionMessage = wideException.getMessage(),
            .sectionsCallStack = {},
        };

        throw;
    }
    catch (const std::exception& exception)
    {
        assertionResult_.exceptionMetadata = ExceptionMetadata{
            .exceptionType = toWideString(getExceptionTypeName(exception)),
            .exceptionMessage = toWideString(exception.what()),
            .sectionsCallStack = {},
        };

        throw;
    }
    catch (...)
    {
        assertionResult_.exceptionMetadata = ExceptionMetadata{
            .exceptionType = L"Unknown",
            .exceptionMessage = L"Unknown",
            .sectionsCallStack = {},
        };

        throw;
    }

    if (!assertionResult_.assertionSuccess)
    {
        THROW(std::runtime_error, "Assertion failed");
    }
}

}
