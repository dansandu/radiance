#include "dansandu/journey/logger.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

using dansandu::journey::logger::LogCritical;
using dansandu::journey::logger::LogDebug;
using dansandu::journey::logger::LogError;
using dansandu::journey::logger::LogInfo;
using dansandu::journey::logger::LogWarning;

TEST_CASE("logging")
{
    SECTION("debug")
    {
        LogInfo("LogDebug");
    }

    SECTION("info")
    {
        LogInfo("InfoMessage");
    }

    SECTION("warning")
    {
        LogWarning("WarningMessage");
    }

    SECTION("error")
    {
        LogError("ErrorMessage");
    }

    SECTION("critical")
    {
        LogCritical("CriticalMessage");
    }
}
