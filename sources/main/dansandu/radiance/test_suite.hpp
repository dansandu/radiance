#pragma once

#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/test_class_descriptor.hpp"
#include "dansandu/journey/common.hpp"

#include <vector>

namespace dansandu::radiance::test_suite
{

TestSuiteResult runTestSuite(
    const std::vector<dansandu::radiance::test_class_descriptor::TestClassDescriptor>& testClassDescriptors,
    const int testClassesTotal,
    const dansandu::journey::Level loggingLevelFailure,
    dansandu::radiance::reporter::IReporter& reporter);

}
