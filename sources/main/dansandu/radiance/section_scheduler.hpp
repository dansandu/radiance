#pragma once

#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/reporter.hpp"

#include <source_location>
#include <sstream>
#include <vector>

namespace dansandu::radiance::section_scheduler
{

class SectionScheduler;

class SectionScope
{
public:
    SectionScope(const wchar_t* const name, const int level, const int index);

    ~SectionScope() noexcept;

    const wchar_t* const name;
    const int level;
    const int index;
    const int exceptions;
    SectionScheduler* scheduler;
};

class PRALINE_EXPORT SectionScheduler
{
public:
    SectionScheduler(dansandu::radiance::reporter::IReporter& reporter);

    void beginRun(const TestCaseRunMetadata& testCaseRunMetadata);

    SectionScope newSection(const wchar_t* const sectionName);

    bool tryBeginSection(SectionScope& sectionScope);

    bool beginSection(const wchar_t* const sectionName);

    void endSection(const wchar_t* const name, const bool failure);

    void endRun();

    bool testCaseDone() const;

    std::wstring getLog() const;

private:
    void debug(const int line);

    void assertThat(const bool assertion, const char* const expression, const int line);

    TestCaseRunMetadata testCaseRunMetadata_;
    std::wostringstream logStream_;
    std::vector<int> sequencer_;
    std::vector<const wchar_t*> sections_;
    std::vector<int> trace_;
    int level_;
    bool testCaseDone_;
    bool seekingSection_;
    bool exiting_;
    bool failure_;
    dansandu::radiance::reporter::IReporter& reporter_;
};

}
