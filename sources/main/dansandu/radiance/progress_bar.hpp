#pragma once

#include "dansandu/radiance/common.hpp"

#include <chrono>
#include <functional>
#include <string>

namespace dansandu::radiance::progress_bar
{

class PRALINE_EXPORT ProgressBar
{
public:
    ProgressBar(const int stageIndex, const int stageCount, const std::wstring& stageName, const int resolution,
                std::function<void(const std::wstring&)> printer, const bool displayElapsedTime);

    ~ProgressBar();

    void updateDescription(const std::wstring& description);

    void updateSummary(const int testsFailed, const int testsSkipped, const int testsPassed,
                       const int assertionsPassed);

    void advance(const int amount = 1);

private:
    void display(const bool firstPrint = false);

    int stageIndex_;
    int stageCount_;
    std::wstring stageName_;
    int resolution_;
    int progress_;
    std::wstring description_;
    std::wstring summary_;
    std::chrono::time_point<std::chrono::steady_clock> startTime_;
    std::function<void(const std::wstring&)> printer_;
    int uncaughtExceptions_;
    bool displayElapsedTime_;
};

}
