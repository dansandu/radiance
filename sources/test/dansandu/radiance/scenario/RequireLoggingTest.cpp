#include "dansandu/journey/logging.hpp"
#include "dansandu/radiance/radiance.hpp"

using dansandu::journey::Level;
using dansandu::radiance::Log;

TEST_CLASS(RequireLoggingTest)
{
public:
    TEST_METHOD(level_and_message_match)
    {
        const auto message = "Some error";

        expectedLogs_.emplace_back(Level::error, message);

        REQUIRE_LOGGING(expectedLogs_, logError(message));
    }

    TEST_METHOD(level_mismatch)
    {
        const auto message = "Another error";

        expectedLogs_.emplace_back(Level::warning, message);

        REQUIRE_LOGGING(expectedLogs_, logError(message));
    }

    TEST_METHOD(message_mismatch)
    {
        expectedLogs_.emplace_back(Level::error, "Expected message");

        REQUIRE_LOGGING(expectedLogs_, logError("Actual message"));
    }

    TEST_METHOD(number_of_logs_mismatch)
    {
        expectedLogs_.emplace_back(Level::error, "Some error");
        expectedLogs_.emplace_back(Level::error, "Some error");

        REQUIRE_LOGGING(expectedLogs_, logError("Some error"));
    }

private:
    // Prevent macro expansion when REQUIRE_LOGGING expression is stringified.
    static void logError(const char* const message)
    {
        LOG_ERROR(message);
    }

    std::vector<Log> expectedLogs_;
};
