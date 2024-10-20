#include "dansandu/radiance/test_case.hpp"
#include "dansandu/radiance/assertion.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/utility.hpp"

using dansandu::radiance::assertion::Assertion;
using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::section_scheduler::SectionScheduler;
using dansandu::radiance::utility::toWideString;

namespace dansandu::radiance::test_case
{

TestCase::TestCase(const TestCaseMetadata& testCaseMetadata, Descriptor descriptor, IReporter& reporter)
    : testCaseResult_{.testCaseMetadata = testCaseMetadata,
                      .assertionsRan = 0,
                      .assertionsPassed = 0,
                      .assertionsFailed = 0,
                      .testCaseSuccess = true},
      testCaseRunResult_{},
      descriptor_{descriptor},
      sectionScheduler_{reporter},
      reporter_{reporter}
{
    reporter_.testCaseBegin(testCaseResult_.testCaseMetadata);
}

TestCase::~TestCase() noexcept
{
    reporter_.testCaseEnd(testCaseResult_);
}

void TestCase::run()
{
    const auto testCaseRunMetadata = TestCaseRunMetadata{
        .testCaseMetadata = testCaseResult_.testCaseMetadata,
    };

    do
    {
        testCaseRunResult_ = TestCaseRunResult{
            .testCaseRunMetadata = testCaseRunMetadata,
            .assertionsRan = 0,
            .assertionsPassed = 0,
            .assertionsFailed = 0,
            .testCaseRunSuccess = false,
        };

        reporter_.testCaseRunBegin(testCaseRunMetadata);

        sectionScheduler_.beginRun(testCaseRunMetadata);

        try
        {
            descriptor_.invoker(*this);

            testCaseRunResult_.testCaseRunSuccess = true;
        }
        catch (const std::exception& exception)
        {
            testCaseRunResult_.exceptionMetadata = ExceptionMetadata{
                .exceptionType = toWideString(typeid(exception).name()),
                .exceptionMessage = toWideString(exception.what()),
            };
        }
        catch (...)
        {
            testCaseRunResult_.exceptionMetadata = ExceptionMetadata{
                .exceptionType = L"Unknown",
                .exceptionMessage = L"Unknown",
            };
        }

        testCaseResult_.assertionsRan += testCaseRunResult_.assertionsRan;
        testCaseResult_.assertionsPassed += testCaseRunResult_.assertionsPassed;
        testCaseResult_.assertionsFailed += testCaseRunResult_.assertionsFailed;
        testCaseResult_.testCaseSuccess = testCaseResult_.testCaseSuccess && testCaseRunResult_.testCaseRunSuccess;

        sectionScheduler_.endRun();

        reporter_.testCaseRunEnd(testCaseRunResult_);

    } while (!sectionScheduler_.testCaseDone());
}

void TestCase::handleAssertion(const char* const expressionString, const int lineNumber,
                               const std::function<void(AssertionResult&)>& expression)
{
    ++testCaseRunResult_.assertionsRan;

    const auto assertionMetadata = AssertionMetadata{.sectionMetadata = sectionScheduler_.currentSection(),
                                                     .expression = expressionString,
                                                     .lineNumber = lineNumber};

    auto assertion = Assertion{assertionMetadata, reporter_};

    try
    {
        assertion.invoke(expression);

        ++testCaseRunResult_.assertionsPassed;
    }
    catch (...)
    {
        ++testCaseRunResult_.assertionsFailed;

        throw;
    }
}

SectionScheduler& TestCase::sectionScheduler()
{
    return sectionScheduler_;
}

const TestCaseResult& TestCase::testCaseResult() const
{
    return testCaseResult_;
}

}
