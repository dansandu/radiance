#pragma once

#include "dansandu/journey/reporter.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"

#include <functional>
#include <typeinfo>
#include <vector>

namespace dansandu::radiance::assertion
{

class PRALINE_EXPORT Assertion
{
public:
    Assertion(const AssertionMetadata& assertionMetadata, dansandu::radiance::reporter::IReporter& reporter);

    ~Assertion() noexcept;

    void invoke(const std::function<void(AssertionResult&)>& expression);

    void logInvoke(
        const dansandu::journey::reporter::InMemoryReporter& inMemoryReporter, const std::vector<Log>& expectedLogs,
        const std::function<void()>& expression
    );

    void throwInvoke(const std::function<void()>& expression, const std::type_info& expectedExceptionType);

private:
    AssertionResult assertionResult_;
    dansandu::radiance::reporter::IReporter& reporter_;
};

}
