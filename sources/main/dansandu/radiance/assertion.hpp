#pragma once

#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/binding.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/utility.hpp"

#include <functional>
#include <type_traits>
#include <typeinfo>

using dansandu::journey::utility::toWideString;

namespace dansandu::radiance::assertion
{

class PRALINE_EXPORT Assertion
{
public:
    Assertion(const AssertionMetadata& assertionMetadata, dansandu::radiance::reporter::IReporter& reporter);

    ~Assertion() noexcept;

    void invoke(const std::function<void(AssertionResult&)>& expression);

    template<typename ExpectedException>
    void throwInvoke(const std::function<void()>& expression)
    {
        using dansandu::journey::exception::WideException;

        using DecayedExpectedException = std::decay_t<ExpectedException>;

        const auto& expectedExceptionTypeId = typeid(DecayedExpectedException);

        if constexpr (std::is_same_v<DecayedExpectedException, std::exception>)
        {
            try
            {
                expression();
            }
            catch (const std::exception& exception)
            {
                const auto& exceptionTypeId = typeid(exception);

                const auto wideException = dynamic_cast<const WideException*>(&exception);

                const auto message = wideException ? wideException->getMessage() : toWideString(exception.what());

                assertionResult_.assertion = ThrowAssertion{
                    .exceptionMessage = message,
                    .expectedException = expectedExceptionTypeId.name(),
                    .actualException = exceptionTypeId.name(),
                    .exceptionThrown = true,
                };

                if (exceptionTypeId == expectedExceptionTypeId)
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
                    .exceptionMessage = L"Unknown",
                    .expectedException = expectedExceptionTypeId.name(),
                    .actualException = "Unknown",
                    .exceptionThrown = true,
                };

                throw;
            }
        }
        else
        {
            try
            {
                expression();
            }
            catch (const DecayedExpectedException& exception)
            {
                const auto& exceptionTypeId = typeid(exception);

                if constexpr (std::is_base_of_v<std::exception, DecayedExpectedException>)
                {
                    const auto wideException = dynamic_cast<const WideException*>(&exception);

                    const auto message = wideException ? wideException->getMessage() : toWideString(exception.what());

                    assertionResult_.assertion = ThrowAssertion{
                        .exceptionMessage = message,
                        .expectedException = expectedExceptionTypeId.name(),
                        .actualException = exceptionTypeId.name(),
                        .exceptionThrown = true,
                    };
                }
                else
                {
                    assertionResult_.assertion = ThrowAssertion{
                        .exceptionMessage = L"Unknown",
                        .expectedException = expectedExceptionTypeId.name(),
                        .actualException = exceptionTypeId.name(),
                        .exceptionThrown = true,
                    };
                }

                if (exceptionTypeId == expectedExceptionTypeId)
                {
                    assertionResult_.assertionSuccess = true;
                }
                else
                {
                    throw;
                }
            }
            catch (const std::exception& exception)
            {
                const auto& exceptionTypeId = typeid(exception);

                const auto wideException = dynamic_cast<const WideException*>(&exception);

                const auto message = wideException ? wideException->getMessage() : toWideString(exception.what());

                assertionResult_.assertion = ThrowAssertion{
                    .exceptionMessage = message,
                    .expectedException = expectedExceptionTypeId.name(),
                    .actualException = exceptionTypeId.name(),
                    .exceptionThrown = true,
                };

                if (exceptionTypeId == expectedExceptionTypeId)
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
                    .exceptionMessage = L"Unknown",
                    .expectedException = expectedExceptionTypeId.name(),
                    .actualException = "Unknown",
                    .exceptionThrown = true,
                };

                throw;
            }
        }

        if (!assertionResult_.assertionSuccess)
        {
            assertionResult_.assertion = ThrowAssertion{
                .expectedException = expectedExceptionTypeId.name(),
                .exceptionThrown = false,
            };

            throw std::runtime_error{"Assertion failed"};
        }
    }

private:
    AssertionResult assertionResult_;
    dansandu::radiance::reporter::IReporter& reporter_;
};

}
