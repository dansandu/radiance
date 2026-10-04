#include "dansandu/journey/logging.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::journey::Level;
using dansandu::radiance::Log;

TEST_CASE("log_assertion")
{
    const auto errorFunction = []() { LOG_ERROR("Some error"); };

    SECTION("match")
    {
        const auto expectedLogs = std::vector<Log>{
            Log{Level::error, L"Some error"},
        };

        REQUIRE_LOG(expectedLogs, errorFunction());
    }

    SECTION("level mismatch")
    {
        const auto expectedLogs = std::vector<Log>{
            Log{Level::warning, L"Some error"},
        };

        REQUIRE_LOG(expectedLogs, errorFunction());
    }

    SECTION("message mismatch")
    {
        const auto expectedLogs = std::vector<Log>{
            Log{Level::error, L"Some warning"},
        };

        REQUIRE_LOG(expectedLogs, errorFunction());
    }

    SECTION("count mismatch")
    {
        const auto expectedLogs = std::vector<Log>{
            Log{Level::error, L"Some error"},
            Log{Level::error, L"Some error"},
        };

        REQUIRE_LOG(expectedLogs, errorFunction());
    }
}
