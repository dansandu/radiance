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
        const auto failure = uncaughtExceptions_ < std::uncaught_exceptions();
        scheduler->endSection(failure);
    }
}

SectionScheduler::SectionScheduler(IReporter& reporter)
    : testCaseRunMetadata_{},
      level_{0},
      testCaseDone_{false},
      seekingSection_{false},
      exiting_{false},
      failure_{false},
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

    testCaseDone_ = true;

    seekingSection_ = false;

    exiting_ = false;

    failure_ = false;
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

    if (!exiting_ || seekingSection_)
    {
        if (trace_[level_] == 0 || (seekingSection_ && trace_[level_] < sectionIndex))
        {
            trace_[level_] = sectionIndex;

            seekingSection_ = false;
            testCaseDone_ = false;
        }
    }

    return SectionScope{sectionName, sectionLevel, sectionIndex};
}

bool SectionScheduler::tryBeginSection(SectionScope& sectionScope)
{
    if (exiting_)
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

void SectionScheduler::endSection(const bool failure)
{
    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(level_ > 0);

    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(std::ssize(sequencer_) > 0);

    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(std::ssize(sections_) > 0);

    --level_;

    auto sectionsCopy = sections_;

    sequencer_.pop_back();

    sections_.pop_back();

    if (!exiting_)
    {
        exiting_ = true;
        failure_ = failure;

        if (!failure_)
        {
            seekingSection_ = true;
        }
    }

    if (seekingSection_)
    {
        DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(std::ssize(trace_) > 0);

        trace_.pop_back();
    }

    DANSANDU_RADIANCE_INTERNAL_DEBUG();

    reporter_.sectionEnd(SectionResult{
        .sectionMetadata =
            {
                .testCaseRunMetadata = testCaseRunMetadata_,
                .sections = std::move(sectionsCopy),
            },
        .sectionSuccess = !failure_,
    });
}

void SectionScheduler::endRun()
{
    logStream_ << "END RUN" << std::endl;

    DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(std::ssize(trace_) > 0);

    if (failure_)
    {
        if (std::ssize(trace_) == 1 && trace_[0] == 0)
        {
            testCaseDone_ = true;
        }
        else
        {
            DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(std::ssize(trace_) > 1);

            DANSANDU_RADIANCE_INTERNAL_ASSERT_THAT(trace_.back() == 0);

            testCaseDone_ = false;

            trace_.pop_back();
            ++trace_.back();
        }
    }

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
               << "], seek: " << seekingSection_ << ", exit: " << exiting_ << ", failure: " << failure_ << std::endl;
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
