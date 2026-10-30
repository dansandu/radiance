#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/logging.hpp"
#include "dansandu/journey/reporter.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/exception.hpp"
#include "dansandu/radiance/library.hpp"
#include "dansandu/radiance/macro.hpp"
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
using dansandu::journey::logging::ReporterGuard;
using dansandu::journey::reporter::standardOutputLogReporter;
using dansandu::radiance::library::getClassMethods;
using dansandu::radiance::library::invokeVoidClassMethod;
using dansandu::radiance::macro::radianceTestMethodPrefix;
using dansandu::radiance::test_class_registry::TestClassRegistry;

namespace
{

int runScenariosVersionTwo(const int numberOfArguments, const char* const* const arguments)
{
    const auto reporterGuard = ReporterGuard{Logger::getGlobalInstance(), "StandardOutput", Level::debug, standardOutputLogReporter};

    if (numberOfArguments < 2)
    {
        LOG_ERROR("Invalid number of program arguments");

        return 1;
    }

    const auto libraryName = std::filesystem::path{arguments[1]}.filename().string();

    const auto testClassDescriptors = TestClassRegistry::getGlobalInstance().getTestClassDescriptors();

    for (const auto& testClassDescriptor : testClassDescriptors)
    {
        LOG_INFO("Loading class method symbols for ", testClassDescriptor.testClassName, " ",
                 testClassDescriptor.testClassFilePath, "(", testClassDescriptor.testClassLineNumber, ")");

        const auto symbols = getClassMethods(libraryName, testClassDescriptor.testClassName, radianceTestMethodPrefix);

        for (const auto& symbol : symbols)
        {
            const auto testClass = testClassDescriptor.testClassProducer();

            invokeVoidClassMethod(symbol, testClass.get());
        }
    }

    return 0;
}

}

DANSANDU_SERVICE_RUNNER_REGISTER_SERVICE("dansandu-radiance-radiance_scenarios_v2", runScenariosVersionTwo);
