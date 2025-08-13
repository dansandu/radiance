#include "dansandu/radiance/progress_bar.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::radiance::progress_bar::formatDuration;

TEST_CASE("progress_bar")
{
    REQUIRE(formatDuration(28LL) == L"28ms");

    REQUIRE(formatDuration(1760LL) == L"1s 760ms");

    REQUIRE(formatDuration(10250LL) == L"10s 250ms");

    REQUIRE(formatDuration(447058LL) == L"7m 27s");

    REQUIRE(formatDuration(7130900LL) == L"1h 58m");

    REQUIRE(formatDuration(172810000LL) == L"2d");

    REQUIRE(formatDuration(950399999LL) == L"10d 23h");
}
