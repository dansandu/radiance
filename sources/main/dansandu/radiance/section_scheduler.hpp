#pragma once

#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"

#include <source_location>
#include <sstream>
#include <vector>

namespace dansandu::radiance::section_scheduler
{

class SectionScheduler;

class PRALINE_EXPORT SectionScope
{
public:
    SectionScope(const std::wstring& name, const int level, const int index);

    ~SectionScope() noexcept;

    const std::wstring name;
    const int level;
    const int index;
    const int uncaughtExceptions_;
    SectionScheduler* scheduler;
};

class PRALINE_EXPORT SectionScheduler
{
public:
    SectionScheduler(dansandu::radiance::reporter::IReporter& reporter);

    ~SectionScheduler() noexcept;

    void beginRun(const TestCaseRunMetadata& testCaseRunMetadata);

    SectionScope newSection(const std::string& sectionName);

    SectionScope newSection(const std::wstring& sectionName);

    bool tryBeginSection(SectionScope& sectionScope);

    void endSection(const bool failure);

    void endRun();

    SectionMetadata currentSection() const;

    bool testCaseDone() const;

    std::wstring getLog() const;

private:
    void debug(const int line);

    void assertThat(const bool assertion, const char* const expression, const int line);

    TestCaseRunMetadata testCaseRunMetadata_;
    std::wostringstream logStream_;
    std::vector<int> sequencer_;
    std::vector<std::wstring> sections_;
    std::vector<int> trace_;
    int level_;
    bool testCaseDone_;
    bool seekingSection_;
    bool exiting_;
    bool failure_;
    dansandu::radiance::reporter::IReporter& reporter_;
};

}
