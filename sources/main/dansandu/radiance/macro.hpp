#pragma once

#include "dansandu/radiance/binding.hpp"
#include "dansandu/radiance/section_scheduler.hpp"
#include "dansandu/radiance/test_case.hpp"
#include "dansandu/radiance/test_case_registry.hpp"
#include "dansandu/radiance/utility.hpp"

#define DANSANDU_RADIANCE_INTERNAL_CONCATENATE_INNER(a, b) a##b

#define DANSANDU_RADIANCE_INTERNAL_CONCATENATE(a, b) DANSANDU_RADIANCE_INTERNAL_CONCATENATE_INNER(a, b)

#define DANSANDU_RADIANCE_INTERNAL_UNIQUE_NAME(prefix) DANSANDU_RADIANCE_INTERNAL_CONCATENATE(prefix, __COUNTER__)

#define DANSANDU_RADIANCEE_INTERNAL_TEST_CASE(name, functionName)                                                      \
    static void functionName(dansandu::radiance::test_case::TestCase&);                                                \
    static bool DANSANDU_RADIANCE_INTERNAL_UNIQUE_NAME(dansandu_radiance_test_case_registrar_) =                       \
        dansandu::radiance::test_case_registry::TestCaseRegistry::instance().registerTestCase({                        \
            .testCaseName = dansandu::radiance::utility::toWideString(name),                                           \
            .filePath = __FILE__,                                                                                      \
            .lineNumber = __LINE__,                                                                                    \
            .invoker = functionName,                                                                                   \
        });                                                                                                            \
    static void functionName(dansandu::radiance::test_case::TestCase& dansandu_radiance_internal_test_case)

#define TEST_CASE(name)                                                                                                \
    DANSANDU_RADIANCEE_INTERNAL_TEST_CASE(name, DANSANDU_RADIANCE_INTERNAL_UNIQUE_NAME(dansandu_radiance_test_case_))

#define SECTION(name)                                                                                                  \
    if (auto dansandu_radiance_internal_sectionScope =                                                                 \
            dansandu_radiance_internal_test_case.sectionScheduler().newSection(name);                                  \
        dansandu_radiance_internal_test_case.sectionScheduler().tryBeginSection(                                       \
            dansandu_radiance_internal_sectionScope))

#if defined(__clang__)
#define REQUIRE(...)                                                                                                   \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Woverloaded-shift-op-parentheses\"")         \
        dansandu_radiance_internal_test_case.handleAssertion(                                                          \
            #__VA_ARGS__, __LINE__,                                                                                    \
            [&](dansandu::radiance::AssertionResult& dansandu_radiance_internal_assertionResult)                       \
            {                                                                                                          \
                auto dansandu_radiance_internal_result = dansandu::radiance::binding::ArgumentBinder{}                 \
                                                             << __VA_ARGS__ >>                                         \
                                                         dansandu::radiance::binding::ArgumentBinder{};                \
                dansandu_radiance_internal_assertionResult.assertion =                                                 \
                    std::move(dansandu_radiance_internal_result.first);                                                \
                dansandu_radiance_internal_assertionResult.assertionSuccess =                                          \
                    dansandu_radiance_internal_result.second;                                                          \
            }) _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define REQUIRE(...)                                                                                                   \
    _Pragma("GCC diagnostic push") _Pragma("GCC diagnostic ignored \"-Woverloaded-shift-op-parentheses\"")             \
        dansandu_radiance_internal_test_case.handleAssertion(                                                          \
            #__VA_ARGS__, __LINE__,                                                                                    \
            [&](dansandu::radiance::AssertionResult& dansandu_radiance_internal_assertionResult)                       \
            {                                                                                                          \
                auto dansandu_radiance_internal_result = dansandu::radiance::binding::ArgumentBinder{}                 \
                                                             << __VA_ARGS__ >>                                         \
                                                         dansandu::radiance::binding::ArgumentBinder{};                \
                dansandu_radiance_internal_assertionResult.assertion =                                                 \
                    std::move(dansandu_radiance_internal_result.first);                                                \
                dansandu_radiance_internal_assertionResult.assertionSuccess =                                          \
                    dansandu_radiance_internal_result.second;                                                          \
            }) _Pragma("GCC diagnostic pop")
#elif defined(_MSC_VER)
#define REQUIRE(...)                                                                                                   \
    __pragma(warning(push)) __pragma(warning(disable : 4554)) dansandu_radiance_internal_test_case.handleAssertion(    \
        #__VA_ARGS__, __LINE__,                                                                                        \
        [&](dansandu::radiance::AssertionResult& dansandu_radiance_internal_assertionResult)                           \
        {                                                                                                              \
            auto dansandu_radiance_internal_result = dansandu::radiance::binding::ArgumentBinder{} << __VA_ARGS__ >>   \
                                                     dansandu::radiance::binding::ArgumentBinder{};                    \
            dansandu_radiance_internal_assertionResult.assertion = std::move(dansandu_radiance_internal_result.first); \
            dansandu_radiance_internal_assertionResult.assertionSuccess = dansandu_radiance_internal_result.second;    \
        }) __pragma(warning(pop))
#elif
#error "Unknown compiler"
#endif

#define REQUIRE_THROW(expression, exception)                                                                           \
    dansandu_radiance_internal_test_case.handleThrowAssertion<exception>(#expression, #exception, __LINE__,            \
                                                                         [&]() { expression; })
