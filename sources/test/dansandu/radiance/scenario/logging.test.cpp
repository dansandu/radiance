#include "dansandu/journey/logging.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

TEST_CASE("logging")
{
    SECTION("debug")
    {
        LOG_DEBUG("DebugMessage");
    }

    SECTION("info")
    {
        LOG_INFO("InfoMessage");
    }

    SECTION("warning")
    {
        LOG_WARNING("WarningMessage");
    }

    SECTION("error")
    {
        LOG_ERROR("ErrorMessage");
    }

    SECTION("critical")
    {
        LOG_CRITICAL("CriticalMessage");
    }
}
