#include "dansandu/radiance/assertion.hpp"
#include "dansandu/radiance/binding.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/utility.hpp"

using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::utility::toWideString;

namespace dansandu::radiance::assertion
{

Assertion::Assertion(const AssertionMetadata& assertionMetadata, IReporter& reporter)
    :  assertionResult_{
        .assertionMetadata = assertionMetadata,
        .assertionSuccess = false,
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
    catch (const std::exception& exception)
    {
        assertionResult_.exceptionMetadata = ExceptionMetadata{
            .exceptionType = toWideString(typeid(exception).name()),
            .exceptionMessage = toWideString(exception.what()),
        };

        throw exception;
    }
    catch (...)
    {
        assertionResult_.exceptionMetadata = ExceptionMetadata{
            .exceptionType = L"Unknown",
            .exceptionMessage = L"Unknown",
        };

        throw;
    }

    if (!assertionResult_.assertionSuccess)
    {
        throw std::runtime_error{"Assertion failed"};
    }
}

}
