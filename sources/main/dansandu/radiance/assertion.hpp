#pragma once

#include "dansandu/radiance/binding.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/utility.hpp"

#include <functional>
#include <type_traits>
#include <typeinfo>

using dansandu::radiance::utility::toWideString;

namespace dansandu::radiance::assertion
{

class PRALINE_EXPORT Assertion
{
public:
    Assertion(const AssertionMetadata& assertionMetadata, dansandu::radiance::reporter::IReporter& reporter);

    ~Assertion() noexcept;

    void invoke(const std::function<void(AssertionResult&)>& expression);

    template<typename ExpectedException>
    void throwInvoke(const char* const expectedException, const std::function<void()>& expression)
    {
        if constexpr (std::is_same_v<std::decay_t<ExpectedException>, std::exception>)
        {
            try
            {
                expression();
            }
            catch (const std::exception& exception)
            {
                assertionResult_.assertion = ThrowAssertion{
                    .exceptionMessage = toWideString(exception.what()),
                    .expectedException = expectedException,
                    .exceptionThrown = true,
                };

                assertionResult_.assertionSuccess = true;
            }
            catch (...)
            {
                assertionResult_.assertion = ThrowAssertion{
                    .exceptionMessage = L"Unknown",
                    .expectedException = expectedException,
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
            catch (const ExpectedException& exception)
            {
                assertionResult_.assertion = ThrowAssertion{
                    .exceptionMessage = toWideString(exception.what()),
                    .expectedException = expectedException,
                    .exceptionThrown = true,
                };

                assertionResult_.assertionSuccess = true;
            }
            catch (const std::exception& exception)
            {
                assertionResult_.assertion = ThrowAssertion{
                    .exceptionMessage = toWideString(exception.what()),
                    .expectedException = expectedException,
                    .exceptionThrown = true,
                };

                throw exception;
            }
            catch (...)
            {
                assertionResult_.assertion = ThrowAssertion{
                    .exceptionMessage = L"Unknown",
                    .expectedException = expectedException,
                    .exceptionThrown = true,
                };

                throw;
            }
        }

        if (!assertionResult_.assertionSuccess)
        {
            assertionResult_.assertion = ThrowAssertion{
                .expectedException = expectedException,
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
