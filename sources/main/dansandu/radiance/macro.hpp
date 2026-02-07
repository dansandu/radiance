#pragma once

#include "dansandu/journey/macro.hpp"
#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/binding.hpp"
#include "dansandu/radiance/section_scheduler.hpp"
#include "dansandu/radiance/test_case.hpp"
#include "dansandu/radiance/test_case_registry.hpp"
#include "dansandu/radiance/utility.hpp"

#define DANSANDU_RADIANCE_INTERNAL_TEST_CASE(name, functionName)                                                       \
    static void functionName(dansandu::radiance::test_case::TestCase&);                                                \
    static bool DANSANDU_JOURNEY_UNIQUE_NAME(dansandu_radiance_test_case_registrar_) =                                 \
        dansandu::radiance::test_case_registry::TestCaseRegistry::instance().registerTestCase({                        \
            .testCaseName = dansandu::journey::utility::toWideString(name),                                            \
            .filePath = __FILE__,                                                                                      \
            .lineNumber = __LINE__,                                                                                    \
            .invoker = functionName,                                                                                   \
        });                                                                                                            \
    static void functionName(dansandu::radiance::test_case::TestCase& dansandu_radiance_internal_test_case)

#define TEST_CASE(name)                                                                                                \
    DANSANDU_RADIANCE_INTERNAL_TEST_CASE(name, DANSANDU_JOURNEY_UNIQUE_NAME(dansandu_radiance_test_case_))

#define SECTION(name)                                                                                                  \
    if (auto dansandu_radiance_internal_sectionScope =                                                                 \
            dansandu_radiance_internal_test_case.sectionScheduler().newSection(name);                                  \
        dansandu_radiance_internal_test_case.sectionScheduler().tryBeginSection(                                       \
            dansandu_radiance_internal_sectionScope))

#define DANSANDU_RADIANCE_INTERNAL_ASSERTION(...)                                                                      \
    dansandu_radiance_internal_test_case.handleAssertion(                                                              \
        #__VA_ARGS__, __LINE__,                                                                                        \
        [&](dansandu::radiance::AssertionResult& dansandu_radiance_internal_assertionResult)                           \
        {                                                                                                              \
            auto dansandu_radiance_internal_result = dansandu::radiance::binding::ArgumentBinder{} << __VA_ARGS__ >>   \
                                                     dansandu::radiance::binding::ArgumentBinder{};                    \
            dansandu_radiance_internal_assertionResult.assertion = std::move(dansandu_radiance_internal_result.first); \
            dansandu_radiance_internal_assertionResult.assertionSuccess = dansandu_radiance_internal_result.second;    \
        })

#if defined(__clang__)
#define REQUIRE(...)                                                                                                   \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Woverloaded-shift-op-parentheses\"")         \
        DANSANDU_RADIANCE_INTERNAL_ASSERTION(__VA_ARGS__) _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define REQUIRE(...) DANSANDU_RADIANCE_INTERNAL_ASSERTION(__VA_ARGS__)
#elif defined(_MSC_VER)
#define REQUIRE(...)                                                                                                   \
    __pragma(warning(push)) __pragma(warning(disable : 4554)) DANSANDU_RADIANCE_INTERNAL_ASSERTION(__VA_ARGS__)        \
        __pragma(warning(pop))
#elif
#error "Unknown compiler"
#endif

#define DANSANDU_RADIANCE_INTERNAL_THROW_ASSERTION(exception, expression)                                              \
    dansandu_radiance_internal_test_case.handleThrowAssertion<exception>(#exception ", " #expression, __LINE__,        \
                                                                         [&]() { expression; })

#if defined(__clang__)
#define REQUIRE_THROW(exception, expression)                                                                           \
    _Pragma("clang diagnostic push") _Pragma("clang diagnostic ignored \"-Wunused-value\"")                            \
        DANSANDU_RADIANCE_INTERNAL_THROW_ASSERTION(exception, expression) _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define REQUIRE_THROW(exception, expression) DANSANDU_RADIANCE_INTERNAL_THROW_ASSERTION(exception, expression)
#elif defined(_MSC_VER)
#define REQUIRE_THROW(exception, expression) DANSANDU_RADIANCE_INTERNAL_THROW_ASSERTION(exception, expression)
#elif
#error "Unknown compiler"
#endif
