#include "dansandu/radiance/test_case_registry.hpp"
#include "dansandu/service_runner/service_registry.hpp"

using dansandu::radiance::test_case_registry::runTestSuite;

namespace dansandu::radiance::service
{

DANSANDU_SERVICE_RUNNER_REGISTER_SERVICE("dansandu-radiance-run_tests", runTestSuite);

}
