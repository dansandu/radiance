#include "dansandu/journey/logging.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

using dansandu::journey::logging::LogCritical;
using dansandu::journey::logging::LogDebug;
using dansandu::journey::logging::LogError;
using dansandu::journey::logging::LogInfo;
using dansandu::journey::logging::LogWarning;

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
