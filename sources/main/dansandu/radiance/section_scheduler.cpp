#include "dansandu/radiance/section_scheduler.hpp"
#include "dansandu/radiance/utility.hpp"

#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using dansandu::radiance::reporter::IReporter;
using dansandu::radiance::utility::join;
using dansandu::radiance::utility::toWideString;

namespace dansandu::radiance::section_scheduler
{

#define DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(expression) assertThat(expression, #expression, __LINE__)

#define DANSANDU_RADIANCE_INTERNAL_DEBUG() debug(__LINE__)

SectionScope::SectionScope(const std::wstring& name, const int level, const int index)
    : name{name}, level{level}, index{index}, uncaughtExceptions_{std::uncaught_exceptions()}, scheduler{nullptr}
{
}

SectionScope::~SectionScope() noexcept
{
    if (scheduler)
    {
        const auto success = uncaughtExceptions_ >= std::uncaught_exceptions();
        scheduler->endSection(success);
    }
}

SectionScheduler::SectionScheduler(IReporter& reporter)
    : level_{0},
      exitingRun_{false},
      sectionFailed_{false},
      seekingSection_{false},
      testCaseDone_{true},
      reporter_{reporter}
{
    logStream_ << std::boolalpha;
}

SectionScheduler::~SectionScheduler() noexcept
{
}

void SectionScheduler::beginRun(const TestCaseRunMetadata& testCaseRunMetadata)
{
    testCaseRunMetadata_ = testCaseRunMetadata;

    logStream_ << "BEGIN RUN" << std::endl;

    DANSANDU_RADIANCE_INTERNAL_DEBUG();

    level_ = 0;

    sequencer_.clear();
    sequencer_.push_back(0);

    if (level_ == std::ssize(trace_))
    {
        trace_.push_back(0);
    }

    exitingRun_ = false;

    sectionFailed_ = false;

    seekingSection_ = false;

    testCaseDone_ = true;
}

SectionScope SectionScheduler::newSection(const std::string& sectionName)
{
    return newSection(toWideString(sectionName));
}

SectionScope SectionScheduler::newSection(const std::wstring& sectionName)
{
    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(level_ >= 0);

    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(level_ < std::ssize(sequencer_));

    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(level_ < std::ssize(trace_));

    DANSANDU_RADIANCE_INTERNAL_DEBUG();

    const auto sectionIndex = ++sequencer_[level_];

    const auto sectionLevel = level_;

    if ((!exitingRun_ && trace_[level_] == 0) || (seekingSection_ && trace_[level_] < sectionIndex))
    {
        trace_[level_] = sectionIndex;

        if (seekingSection_)
        {
            seekingSection_ = false;

            testCaseDone_ = false;
        }
    }

    return SectionScope{sectionName, sectionLevel, sectionIndex};
}

bool SectionScheduler::tryBeginSection(SectionScope& sectionScope)
{
    if (exitingRun_)
    {
        return false;
    }

    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(level_ >= 0);

    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(level_ < std::ssize(trace_));

    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(sectionScope.level >= 0);

    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(sectionScope.level < std::ssize(trace_));

    if (sectionScope.level == level_ && sectionScope.index == trace_[sectionScope.level])
    {
        sectionScope.scheduler = this;

        ++level_;

        sequencer_.push_back(0);

        sections_.push_back(sectionScope.name);

        if (level_ == std::ssize(trace_))
        {
            trace_.push_back(0);
        }

        logStream_ << "  SECTION \"" << sectionScope.name << "\"" << std::endl;

        DANSANDU_RADIANCE_INTERNAL_DEBUG();

        reporter_.sectionBegin(SectionMetadata{
            .testCaseRunMetadata = testCaseRunMetadata_,
            .sections = sections_,
        });

        return true;
    }

    return false;
}

void SectionScheduler::endSection(const bool success)
{
    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(level_ > 0);

    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(std::ssize(sequencer_) > 0);

    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(std::ssize(sections_) > 0);

    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(std::ssize(trace_) > 1);

    auto sectionsCopy = sections_;

    --level_;

    sequencer_.pop_back();

    sections_.pop_back();

    if (!exitingRun_)
    {
        exitingRun_ = true;

        if (success)
        {
            seekingSection_ = true;
        }
    }

    if (!success && !sectionFailed_)
    {
        sectionFailed_ = true;

        seekingSection_ = false;

        testCaseDone_ = false;

        trace_.pop_back();

        ++trace_.back();
    }

    if (seekingSection_)
    {
        trace_.pop_back();
    }

    DANSANDU_RADIANCE_INTERNAL_DEBUG();

    reporter_.sectionEnd(SectionResult{
        .sectionMetadata =
            {
                .testCaseRunMetadata = testCaseRunMetadata_,
                .sections = std::move(sectionsCopy),
            },
        .sectionSuccess = !sectionFailed_,
    });
}

void SectionScheduler::endRun()
{
    logStream_ << "END RUN" << std::endl;

    DANSANDU_RADIANCE_INTERNAL_DEBUG();
}

SectionMetadata SectionScheduler::currentSection() const
{
    return SectionMetadata{
        .testCaseRunMetadata = testCaseRunMetadata_,
        .sections = sections_,
    };
}

bool SectionScheduler::testCaseDone() const
{
    return testCaseDone_;
}

void SectionScheduler::debug(const int line)
{
    logStream_ << "    line: " << line << ", level: " << level_ << "], sections: [" << join(sections_)
               << "], sequencer: [" << join(sequencer_) << "], trace: [" << join(trace_)
               << "], exiting run: " << exitingRun_ << ", section failed: " << sectionFailed_
               << ", seeking section: " << seekingSection_ << ", test case done: " << testCaseDone_ << std::endl;
}

std::wstring SectionScheduler::getLog() const
{
    return logStream_.str();
}

void SectionScheduler::assertThat(const bool assertion, const char* const expression, const int line)
{
    if (!assertion)
    {
        const auto logFile = "radiance.log";

        logStream_ << "error at line " << line << " with expression: " << expression << std::endl
                   << "See \"" << logFile << "\" log file for more details." << std::endl;

        const auto str = logStream_.str();

        std::wcerr << str;
        std::wcerr.flush();

        auto file = std::wofstream{logFile};
        file << str;
        file.flush();

        std::terminate();
    }
}

}
