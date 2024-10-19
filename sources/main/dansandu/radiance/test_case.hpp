#pragma once

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
