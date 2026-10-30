#pragma once

#include "dansandu/radiance/progress_bar.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/utility.hpp"

#include <functional>
#include <optional>
#include <sstream>
#include <string>

namespace dansandu::radiance::progress_bar_console_reporter
{

class PRALINE_EXPORT ProgressBarConsoleReporter : public dansandu::radiance::reporter::IReporter
{
public:
    using Printer = std::function<void(const std::string& text)>;

    using Highlighter =
        std::function<std::string(const std::string& text, dansandu::radiance::utility::TextHighlight textHighlight)>;

    ProgressBarConsoleReporter(Printer progressBarPrinter, Printer outputPrinter, Highlighter highlighter);

    void testSuiteBegin(const TestSuiteMetadata& testSuiteMetadata) override;

    void testSuiteEnd(const TestSuiteResult& testSuiteResult) override;

    void testClassBegin(const TestClassMetadata& testClassMetadata) override;

    void testClassEnd(const TestClassResult& testClassResult) override;

    void testMethodBegin(const TestMethodMetadata& testMethodMetadata) override;

    void testMethodEnd(const TestMethodResult& testMethodResult) override;

    void assertionEnd(const AssertionResult& assertionResult) override;

private:
    Printer progressBarPrinter_;
    Printer outputPrinter_;
    Highlighter highlighter_;
    std::ostringstream stream_;
    std::optional<dansandu::radiance::progress_bar::ProgressBar> progressBar_;
    bool failureHandled_;
};

}
