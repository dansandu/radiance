#pragma once

#include "dansandu/journey/common.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/test_class_descriptor.hpp"

#include <map>
#include <set>

namespace dansandu::radiance::test_suite
{

TestSuiteResult runTestSuite(
    const std::set<std::string>& testClassNames,
    const std::map<std::string, dansandu::radiance::test_class_descriptor::TestClassDescriptor>&
        testClassDescriptorsByClassName,
    const dansandu::journey::Level loggingLevelFailure, dansandu::radiance::reporter::IReporter& reporter
);

}
