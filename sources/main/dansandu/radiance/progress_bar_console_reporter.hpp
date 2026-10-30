#pragma once

#include "dansandu/radiance/progress_bar.hpp"
#include "dansandu/radiance/reporter.hpp"

#include <optional>
#include <sstream>

namespace dansandu::radiance::progress_bar_console_reporter
{

class ProgressBarConsoleReporter : public dansandu::radiance::reporter::IReporter
{
public:
    ProgressBarConsoleReporter();

    void testSuiteBegin(const TestSuiteMetadata& testSuiteMetadata) override;

    void testSuiteEnd(const TestSuiteResult& testSuiteResult) override;

    void testClassBegin(const TestClassMetadata& testClassMetadata) override;

    void testClassEnd(const TestClassResult& testClassResult) override;

    void testMethodBegin(const TestMethodMetadata& testMethodMetadata) override;

    void testMethodEnd(const TestMethodResult& testMethodResult) override;

    void assertionEnd(const AssertionResult& assertionResult) override;

private:
    std::ostringstream stream_;
    std::optional<dansandu::radiance::progress_bar::ProgressBar> progressBar_;
    bool failureHandled_;
};

}
