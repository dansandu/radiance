#include "dansandu/journey/logging.hpp"
#include "dansandu/journey/reporter.hpp"
#include "dansandu/radiance/radiance.hpp"

#include <stdexcept>

using dansandu::journey::Level;
using dansandu::journey::logging::Logger;
using dansandu::journey::reporter::InMemoryReporter;
using dansandu::radiance::Log;

TEST_CLASS(RequireLoggingThrowTest)
{
public:
    void setUp() override
    {
        Logger::getGlobalInstance().addReporter(reporterName_, Level::info, memoryReporter_);
    }

    void cleanUp() override
    {
        LOG_WARNING("Logged after throw");

        Logger::getGlobalInstance().removeReporter(reporterName_);

        const auto logs = memoryReporter_.getLoggedEntries();

        // Cast to int because Windows and Linux have different representations for size_t.
        REQUIRE(static_cast<int>(logs.size()) == 2);

        REQUIRE(logs.at(0).level == Level::error);

        REQUIRE(logs.at(0).message == "Logged before throw");

        REQUIRE(logs.at(1).level == Level::warning);

        REQUIRE(logs.at(1).message == "Logged after throw");
    }

    TEST_METHOD(expression_throws)
    {
        const auto expectedLogs = std::vector<Log>{
            Log{Level::error, "Logged before throw"},
        };

        REQUIRE_LOGGING(expectedLogs, logErrorAndThrow("Logged before throw"));
    }

private:
    // Prevent macro expansion when REQUIRE_LOGGING expression is stringified.
    static void logErrorAndThrow(const char* const message)
    {
        LOG_ERROR(message);

        throw std::runtime_error{"Expression failure"};
    }

    static constexpr auto reporterName_ = "RequireLoggingThrowTestReporter";

    InMemoryReporter memoryReporter_;
};
