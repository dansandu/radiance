#include "dansandu/radiance/progress_bar.hpp"

#include <iostream>
#include <thread>

using dansandu::radiance::progress_bar::ProgressBar;

int main(const int, const char* const* const)
{
    const auto resolution = 10;

    auto progressBar = ProgressBar{0, 0, L"main", resolution, [](const auto& text) { std::wcout << text; }, true};
    for (int i = 0; i < resolution; ++i)
    {
        progressBar.updateDescription(std::to_wstring(i));
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        progressBar.advance(1);
    }
    progressBar.updateSummary(15677, 1234, 24535);
    return 0;
}
