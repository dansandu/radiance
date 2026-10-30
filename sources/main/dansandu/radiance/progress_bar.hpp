#pragma once

#include "dansandu/radiance/common.hpp"

#include <chrono>
#include <functional>
#include <string>

namespace dansandu::radiance::progress_bar
{

class ProgressBar
{
public:
    ProgressBar(const std::string& stageName, const int resolution, std::function<void(const std::string&)> printer,
                const bool displayElapsedTime);

    ~ProgressBar();

    void updateDescription(const std::string& description);

    void updateSummary(const int testsFailed, const int testsSkipped, const int testsPassed,
                       const int assertionsPassed);

    void advance(const int amount = 1);

private:
    void display(const bool firstPrint = false);

    std::string stageName_;
    int resolution_;
    int progress_;
    std::string description_;
    std::string summary_;
    std::chrono::time_point<std::chrono::steady_clock> startTime_;
    std::function<void(const std::string&)> printer_;
    int uncaughtExceptions_;
    bool displayElapsedTime_;
};

std::string formatDuration(const long long milliseconds);

}
