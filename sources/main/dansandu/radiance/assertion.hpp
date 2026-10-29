#pragma once

#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/reporter.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/binding.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/utility.hpp"

#include <functional>
#include <type_traits>
#include <typeinfo>

namespace dansandu::radiance::assertion
{

class PRALINE_EXPORT Assertion
{
public:
    Assertion(const AssertionMetadata& assertionMetadata, dansandu::radiance::reporter::IReporter& reporter);

    ~Assertion() noexcept;

    void invoke(const std::function<void(AssertionResult&)>& expression);

    void logInvoke(const dansandu::journey::reporter::InMemoryReporter& testCaseInMemoryReporter,
                   const std::vector<Log>& expectedLogs, const std::function<void()>& expression);

    template<typename ExpectedException>
    void throwInvoke(const std::function<void()>& expression)
    {
        using dansandu::journey::exception::WideException;
        using dansandu::journey::utility::getTypeName;
        using dansandu::journey::utility::toWideString;

        using DecayedExpectedException = std::decay_t<ExpectedException>;

        constexpr auto includeNamespace = true;

        if constexpr (std::is_same_v<DecayedExpectedException, std::exception>)
        {
            try
            {
                expression();
            }
            catch (const std::exception& exception)
            {
                const auto wideException = dynamic_cast<const WideException*>(&exception);

                const auto message = wideException ? wideException->getMessage() : toWideString(exception.what());

                const auto includeExceptionNamespace = true;

                assertionResult_.assertion = ThrowAssertion{
                    .exceptionMessage = message,
                    .expectedException = getTypeName<DecayedExpectedException>(includeExceptionNamespace),
                    .actualException = getTypeName(includeExceptionNamespace, exception),
                    .exceptionThrown = true,
                };

                if (typeid(exception) == typeid(DecayedExpectedException))
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
                    .exceptionMessage = L"<Unknown>",
                    .expectedException = getTypeName<DecayedExpectedException>(includeNamespace),
                    .actualException = "<Unknown>",
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
                if constexpr (std::is_base_of_v<std::exception, DecayedExpectedException>)
                {
                    const auto wideException = dynamic_cast<const WideException*>(&exception);

                    const auto message = wideException ? wideException->getMessage() : toWideString(exception.what());

                    assertionResult_.assertion = ThrowAssertion{
                        .exceptionMessage = message,
                        .expectedException = getTypeName<DecayedExpectedException>(includeNamespace),
                        .actualException = getTypeName(includeNamespace, exception),
                        .exceptionThrown = true,
                    };
                }
                else
                {
                    assertionResult_.assertion = ThrowAssertion{
                        .exceptionMessage = L"<Unknown>",
                        .expectedException = getTypeName<DecayedExpectedException>(includeNamespace),
                        .actualException = getTypeName(includeNamespace, exception),
                        .exceptionThrown = true,
                    };
                }

                if (typeid(exception) == typeid(DecayedExpectedException))
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
                const auto wideException = dynamic_cast<const WideException*>(&exception);

                const auto message = wideException ? wideException->getMessage() : toWideString(exception.what());

                assertionResult_.assertion = ThrowAssertion{
                    .exceptionMessage = message,
                    .expectedException = getTypeName<DecayedExpectedException>(includeNamespace),
                    .actualException = getTypeName(includeNamespace, exception),
                    .exceptionThrown = true,
                };

                if (typeid(exception) == typeid(DecayedExpectedException))
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
                    .exceptionMessage = L"<Unknown>",
                    .expectedException = getTypeName<DecayedExpectedException>(includeNamespace),
                    .actualException = "<Unknown>",
                    .exceptionThrown = true,
                };

                throw;
            }
        }

        if (!assertionResult_.assertionSuccess)
        {
            assertionResult_.assertion = ThrowAssertion{
                .exceptionMessage = {},
                .expectedException = getTypeName<DecayedExpectedException>(includeNamespace),
                .actualException = {},
                .exceptionThrown = false,
            };

            using dansandu::radiance::exception::AssertionFailureException;
            WTHROW(AssertionFailureException, "Assertion failed");
        }
    }

private:
    AssertionResult assertionResult_;
    dansandu::radiance::reporter::IReporter& reporter_;
};

}
