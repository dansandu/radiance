#include "dansandu/radiance/progress_bar.hpp"
#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/utility.hpp"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>

using dansandu::radiance::utility::highlightText;
using dansandu::radiance::utility::join;
using dansandu::radiance::utility::TextHighlight;

namespace dansandu::radiance::progress_bar
{

namespace
{

constexpr auto maximumDescriptionSize = 40;
constexpr auto barSize = 50;
constexpr auto filledBarCharacter = '=';
constexpr auto emptyBarCharacter = '=';
constexpr auto moveCursorUpTwoLines = "\x1B[2F";
constexpr auto deleteTwoLines = "\x1B[2M";
constexpr auto failureText = "failed";

std::string formatDescription(const std::string& description)
{
    const auto descriptionSize = static_cast<int>(description.size());
    if (descriptionSize > maximumDescriptionSize)
    {
        return "..." + description.substr(descriptionSize - maximumDescriptionSize + 3);
    }
    else
    {
        return description;
    }
}

std::string formatPercentage(const double percentage)
{
    if (percentage < 1.0)
    {
        char buffer[8];
        std::snprintf(buffer, 8, "%5.2f%%", percentage * 100.0);
        return buffer;
    }
    else
    {
        return "99.99%";
    }
}

}

std::string formatDuration(const long long milliseconds)
{
    const auto promotions = {
        std::make_pair(1000LL, "s"),
        std::make_pair(60LL, "m"),
        std::make_pair(60LL, "h"),
        std::make_pair(24LL, "d"),
    };

    auto elapsed = milliseconds;
    auto elapsedUnit = "ms";
    auto elapsedRemainder = 0LL;
    auto elapsedRemainderUnit = static_cast<const char*>(nullptr);

    for (const auto& [factor, unit] : promotions)
    {
        if (elapsed >= factor)
        {
            elapsedRemainder = elapsed % factor;
            elapsedRemainderUnit = elapsedUnit;
            elapsed = elapsed / factor;
            elapsedUnit = unit;
        }
        else
        {
            break;
        }
    }

    if (elapsedRemainder > 0)
    {
        return std::to_string(elapsed) + elapsedUnit + " " + std::to_string(elapsedRemainder) + elapsedRemainderUnit;
    }
    else
    {
        return std::to_string(elapsed) + elapsedUnit;
    }
}

ProgressBar::ProgressBar(
    const std::string& stageName, const int resolution, std::function<void(const std::string&)> printer,
    const bool displayElapsedTime
)
    : stageName_{stageName},
      resolution_{resolution},
      progress_{0},
      startTime_{std::chrono::steady_clock::now()},
      printer_{std::move(printer)},
      uncaughtExceptions_{std::uncaught_exceptions()},
      displayElapsedTime_{displayElapsedTime}
{
    if (resolution < 0)
    {
        THROW(std::invalid_argument, "Progress bar resolution must be greater or equal to 0");
    }

    display(true);
}

ProgressBar::~ProgressBar()
{
    const auto success = uncaughtExceptions_ >= std::uncaught_exceptions();

    auto header = std::string{moveCursorUpTwoLines} + deleteTwoLines;

    if (success)
    {
        if (!summary_.empty())
        {
            header = header + summary_;
        }

        if (displayElapsedTime_)
        {
            const auto endTime = std::chrono::steady_clock::now();
            const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
            header = header + " " + formatDuration(elapsed.count());
        }
    }
    else
    {
        header = header + " " + highlightText(failureText, TextHighlight::Red);
    }

    printer_(header + "\n");
}

void ProgressBar::updateDescription(const std::string& description)
{
    description_ = description;
    display();
}

void ProgressBar::updateSummary(
    const int testClassesFailed, const int testClassesSkipped, const int testClassesPassed, const int testMethodsPassed,
    const int assertionsPassed
)
{
    if (testClassesFailed == 0 && testClassesSkipped == 0 && testClassesPassed == 0)
    {
        summary_ = " " + highlightText("No test classes were found", TextHighlight::Yellow);
    }
    else
    {
        auto metrics = std::vector<std::string>{};

        if (testClassesFailed > 0)
        {
            metrics.push_back(highlightText(std::to_string(testClassesFailed) + " classes failed", TextHighlight::Red));
        }

        if (testClassesSkipped > 0)
        {
            metrics.push_back(
                highlightText(std::to_string(testClassesSkipped) + " classes skipped", TextHighlight::Yellow)
            );
        }

        metrics.push_back(highlightText(std::to_string(testClassesPassed) + " classes passed", TextHighlight::Green));

        if (testClassesFailed == 0 && testClassesSkipped == 0 && testClassesPassed > 0)
        {
            metrics.push_back(
                highlightText(std::to_string(testMethodsPassed) + " methods passed", TextHighlight::Green)
            );

            metrics.push_back(
                highlightText(std::to_string(assertionsPassed) + " assertions passed", TextHighlight::Green)
            );
        }

        summary_ += " test " + join(metrics, " | ");
    }

    display();
}

void ProgressBar::advance(const int amount)
{
    if (resolution_ == 0)
    {
        THROW(
            std::invalid_argument,
            "Cannot advance if progress bar resolution is 0 -- the bar will be filled on successful deconstruction"
        );
    }

    if (amount <= 0)
    {
        THROW(std::invalid_argument, "Progress bar advance amount must be greater than 0");
    }

    progress_ = std::min(progress_ + amount, resolution_);
    display();
}

void ProgressBar::display(const bool firstPrint)
{
    auto header = " " + stageName_;

    if (!description_.empty())
    {
        header = header + " " + formatDescription(description_);
    }

    if (!firstPrint)
    {
        header = std::string{moveCursorUpTwoLines} + deleteTwoLines + header;
    }

    auto bar = std::string{};
    auto footer = std::string{};

    if (resolution_ > 0)
    {
        const auto percentage = static_cast<double>(progress_) / resolution_;
        const auto filledSize = std::min(static_cast<int>(percentage * barSize), barSize - 1);
        bar = highlightText(std::string(filledSize, filledBarCharacter), TextHighlight::Blue) +
              std::string(barSize - filledSize, emptyBarCharacter);
        footer = "  " + bar + "  " + formatPercentage(percentage);
    }
    else
    {
        bar = std::string(barSize, emptyBarCharacter);
        footer = "  " + bar;
    }

    printer_(header + "\n" + footer + "\n");
}

}
