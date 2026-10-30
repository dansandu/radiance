#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/logging.hpp"
#include "dansandu/journey/reporter.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/progress_bar.hpp"
#include "dansandu/radiance/test_class_registry.hpp"
#include "dansandu/radiance/utility.hpp"
#include "dansandu/service_runner/service_registry.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>

using dansandu::journey::Level;
using dansandu::journey::logging::Logger;
using dansandu::journey::reporter::standardOutputLogReporter;
using dansandu::radiance::test_class_registry::TestClassRegistry;

namespace
{

int runScenariosVersionTwo(const int, const char* const* const)
{
    const auto& logger = Logger::getGlobalInstance();
    logger.addReporter(L"StandardOutput", Level::debug, standardOutputLogReporter);

    const auto testClassDescriptors = TestClassRegistry::getGlobalInstance().getTestClassDescriptors();

    for (const auto& testClassDescriptor : testClassDescriptors)
    {
        const auto testClass = testClassDescriptor.testClassFactory();

        LOG_INFO(testClassDescriptor.testClassName);
    }

    // Remove reporter otherwise it dangles when the library is unloaded, leading to memory access violation.
    logger.removeReporter(L"StandardOutput");

    return 0;
}

}

DANSANDU_SERVICE_RUNNER_REGISTER_SERVICE("dansandu-radiance-radiance_scenarios_v2", runScenariosVersionTwo);
