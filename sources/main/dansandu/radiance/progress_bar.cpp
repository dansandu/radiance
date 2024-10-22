#include "dansandu/radiance/progress_bar.hpp"
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
using dansandu::radiance::utility::toWideString;

namespace dansandu::radiance::progress_bar
{

static constexpr auto maximumDescriptionSize = 40;
static constexpr auto barSize = 50;
static constexpr auto filledBarCharacter = L'=';
static constexpr auto emptyBarCharacter = L'=';
static constexpr auto moveCursorUpTwoLines = L"\x1B[2F";
static constexpr auto deleteTwoLines = L"\x1B[2M";
static constexpr auto failureText = L"failed";

static std::wstring formatIndex(const int stageIndex, const int stageCount)
{
    const auto index = std::to_wstring(stageIndex);
    const auto count = std::to_wstring(stageCount);
    const auto padding = std::wstring(count.size() - index.size(), L' ');
    return padding + L"(" + index + L"/" + count + L")";
}

static std::wstring formatDescription(const std::wstring& description)
{
    const auto descriptionSize = static_cast<int>(description.size());
    if (descriptionSize > maximumDescriptionSize)
    {
        return L"..." + description.substr(descriptionSize - maximumDescriptionSize + 3);
    }
    else
    {
        return description;
    }
}

static std::wstring formatDuration(const long long milliseconds)
{
    const auto promotions = {
        std::make_pair(1000.0, L"s"),
        std::make_pair(60.0, L"m"),
        std::make_pair(60.0, L"h"),
        std::make_pair(24.0, L"d"),
    };

    auto elapsed = static_cast<double>(milliseconds);
    auto elapsedUnit = L"ms";
    auto elapsedFraction = 0.0;
    auto elapsedFrationUnit = static_cast<const wchar_t*>(nullptr);

    for (const auto [factor, unit] : promotions)
    {
        if (elapsed > factor)
        {
            elapsedFraction = std::fmod(elapsed, factor);
            elapsedFrationUnit = elapsedUnit;
            elapsed = std::floor(elapsed / factor);
            elapsedUnit = unit;
        }
        else
        {
            break;
        }
    }

    auto result = std::to_wstring(static_cast<long long>(elapsed)) + elapsedUnit;

    if (static_cast<long long>(elapsedFraction) > 0)
    {
        result += L" " + std::to_wstring(static_cast<long long>(elapsedFraction)) + elapsedFrationUnit;
    }

    return result;
}

static std::wstring formatPercentage(const double percentage)
{
    if (percentage < 1.0)
    {
        char buffer[8];
        std::snprintf(buffer, 8, "%5.2f%%", percentage * 100.0);
        return toWideString(buffer);
    }
    else
    {
        return L"99.99%";
    }
}

ProgressBar::ProgressBar(const int stageIndex, const int stageCount, const std::wstring& stageName,
                         const int resolution, std::function<void(const std::wstring&)> printer,
                         const bool displayElapsedTime)
    : stageIndex_{stageIndex},
      stageCount_{stageCount},
      stageName_{stageName},
      resolution_{resolution},
      progress_{0},
      startTime_{std::chrono::steady_clock::now()},
      printer_{std::move(printer)},
      uncaughtExceptions_{std::uncaught_exceptions()},
      displayElapsedTime_{displayElapsedTime}
{
    if (resolution < 0)
    {
        throw std::invalid_argument{"progress bar resolution must be greater or equal to 0"};
    }

    display(true);
}

ProgressBar::~ProgressBar()
{
    const auto success = uncaughtExceptions_ >= std::uncaught_exceptions();

    auto header = std::wstring{moveCursorUpTwoLines} + deleteTwoLines;

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
            header = header + L" " + formatDuration(elapsed.count());
        }
    }
    else
    {
        header = header + L" " + highlightText(failureText, TextHighlight::Red);
    }

    printer_(header + L"\n");
}

void ProgressBar::updateDescription(const std::wstring& description)
{
    description_ = description;
    display();
}

void ProgressBar::updateSummary(const int testsFailed, const int testsSkipped, const int testsPassed,
                                const int assertionsPassed)
{
    if (testsFailed == 0 && testsSkipped == 0 && testsPassed == 0)
    {
        summary_ =
            formatIndex(stageIndex_, stageCount_) + L" " + highlightText(L"No tests found", TextHighlight::Yellow);
    }
    else
    {
        auto metrics = std::vector<std::wstring>{};

        if (testsFailed > 0)
        {
            metrics.push_back(highlightText(std::to_wstring(testsFailed) + L" tests failed", TextHighlight::Red));
        }

        if (testsSkipped > 0)
        {
            metrics.push_back(highlightText(std::to_wstring(testsSkipped) + L" tests skipped", TextHighlight::Yellow));
        }

        metrics.push_back(highlightText(std::to_wstring(testsPassed) + L" tests passed", TextHighlight::Green));

        if (testsFailed == 0 && testsSkipped == 0 && testsPassed > 0)
        {
            metrics.push_back(
                highlightText(std::to_wstring(assertionsPassed) + L" assertions passed", TextHighlight::Green));
        }

        summary_ += formatIndex(stageIndex_, stageCount_) + L" test " + join(metrics, L" | ");
    }

    display();
}

void ProgressBar::advance(const int amount)
{
    if (resolution_ == 0)
    {
        throw std::invalid_argument{"cannot advance if progress bar resolution is 0 -- the bar will be filled on "
                                    "successful deconstruction"};
    }

    if (amount <= 0)
    {
        throw std::invalid_argument{"progress bar advance amount must be greater than 0"};
    }

    progress_ = std::min(progress_ + amount, resolution_);
    display();
}

void ProgressBar::display(const bool firstPrint)
{
    auto header = formatIndex(stageIndex_, stageCount_) + L" " + stageName_;

    if (!description_.empty())
    {
        header = header + L" " + formatDescription(description_);
    }

    if (!firstPrint)
    {
        header = std::wstring{moveCursorUpTwoLines} + deleteTwoLines + header;
    }

    auto bar = std::wstring{};
    auto footer = std::wstring{};

    if (resolution_ > 0)
    {
        const auto percentage = static_cast<double>(progress_) / resolution_;
        const auto filledSize = std::min(static_cast<int>(percentage * barSize), barSize - 1);
        bar = highlightText(std::wstring(filledSize, filledBarCharacter), TextHighlight::Blue) +
              std::wstring(barSize - filledSize, emptyBarCharacter);
        footer = L"  " + bar + L"  " + formatPercentage(percentage);
    }
    else
    {
        bar = std::wstring(barSize, emptyBarCharacter);
        footer = L"  " + bar;
    }

    printer_(header + L"\n" + footer + L"\n");
}

}
