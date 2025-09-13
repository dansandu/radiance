#pragma once

#include "dansandu/radiance/progress_bar.hpp"
#include "dansandu/radiance/reporter.hpp"

#include <optional>
#include <sstream>

namespace dansandu::radiance::progress_bar_console_reporter
{

class PRALINE_EXPORT ProgressBarConsoleReporter : public dansandu::radiance::reporter::IReporter
{
public:
    ProgressBarConsoleReporter();

    void testSuiteBegin(const TestSuiteMetadata& metadata) override;

    void testSuiteEnd(const TestSuiteResult& result) override;

    void testCaseBegin(const TestCaseMetadata& metadata) override;

    void testCaseEnd(const TestCaseResult& result) override;

    void testCaseRunBegin(const TestCaseRunMetadata& metadata) override;

    void testCaseRunEnd(const TestCaseRunResult& result) override;

    void assertionEnd(const AssertionResult& result) override;

private:
    std::wostringstream stream_;
    std::optional<dansandu::radiance::progress_bar::ProgressBar> progressBar_;
    bool failureHandled_;
};

}
