#pragma once

#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"
#include "dansandu/radiance/test_case.hpp"

#include <vector>

namespace dansandu::radiance::test_suite
{

class PRALINE_EXPORT TestSuite
{
public:
    TestSuite(const TestSuiteMetadata& testSuiteMetadata,
              const std::vector<dansandu::radiance::test_case::TestCase::Descriptor>& testCaseDescriptors,
              dansandu::radiance::reporter::IReporter& reporter);

    ~TestSuite() noexcept;

    void run();

    const TestSuiteResult& testSuiteResult() const;

private:
    TestSuiteResult testSuiteResult_;
    std::vector<dansandu::radiance::test_case::TestCase::Descriptor> testCaseDescriptors_;
    dansandu::radiance::reporter::IReporter& reporter_;
};

}
