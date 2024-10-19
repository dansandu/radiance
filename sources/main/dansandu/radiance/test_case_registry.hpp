#pragma once

#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/test_case.hpp"

#include <string>
#include <vector>

namespace dansandu::radiance::test_case_registry
{

class PRALINE_EXPORT TestCaseRegistry
{
public:
    static TestCaseRegistry& instance();

    bool registerTestCase(dansandu::radiance::test_case::TestCase::Descriptor testCaseDescriptor);

    TestSuiteResult runTestCase(const std::wstring& testCaseName,
                                dansandu::radiance::reporter::IReporter& reporter) const;

    TestSuiteResult runAllTestCases(dansandu::radiance::reporter::IReporter& reporter) const;

private:
    std::vector<dansandu::radiance::test_case::TestCase::Descriptor> testCaseDescriptors_;
};

}
