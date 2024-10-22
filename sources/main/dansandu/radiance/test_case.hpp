#pragma once

#include "dansandu/radiance/assertion.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/section_scheduler.hpp"

#include <functional>
#include <string>
#include <type_traits>
#include <typeinfo>

namespace dansandu::radiance::test_case
{

class PRALINE_EXPORT TestCase
{
public:
    struct Descriptor
    {
        std::wstring testCaseName;
        const char* filePath;
        int lineNumber;
        void (*invoker)(TestCase&);
    };

    TestCase(const TestCaseMetadata& testCaseMetadata, Descriptor descriptor,
             dansandu::radiance::reporter::IReporter& reporter);

    ~TestCase() noexcept;

    void run();

    void handleAssertion(const char* const expressionString, const int lineNumber,
                         const std::function<void(AssertionResult&)>& expression);

    template<typename ExpectedException>
    void handleThrowAssertion(const char* const expressionString, const char* const expectedException,
                              const int lineNumber, const std::function<void()>& expression)
    {
        ++testCaseRunResult_.assertionsRan;

        const auto assertionMetadata = AssertionMetadata{.sectionMetadata = sectionScheduler_.currentSection(),
                                                         .expression = expressionString,
                                                         .lineNumber = lineNumber};

        auto assertion = dansandu::radiance::assertion::Assertion{assertionMetadata, reporter_};

        try
        {
            assertion.throwInvoke<ExpectedException>(expectedException, expression);

            ++testCaseRunResult_.assertionsPassed;
        }
        catch (...)
        {
            ++testCaseRunResult_.assertionsFailed;

            throw;
        }
    }

    dansandu::radiance::section_scheduler::SectionScheduler& sectionScheduler();

    const TestCaseResult& testCaseResult() const;

private:
    TestCaseResult testCaseResult_;
    TestCaseRunResult testCaseRunResult_;
    Descriptor descriptor_;
    dansandu::radiance::section_scheduler::SectionScheduler sectionScheduler_;
    dansandu::radiance::reporter::IReporter& reporter_;
};

}
