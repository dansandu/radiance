#pragma once

#include "dansandu/journey/macro.hpp"
#include "dansandu/radiance/binding.hpp"
#include "dansandu/radiance/registered_test_class.hpp"
#include "dansandu/radiance/test_method_context.hpp"

#define DANSANDU_RADIANCE_INTERNAL_ASSERTION(...)                                                                      \
    BaseTestClass::handleAssertion(                                                                                    \
        #__VA_ARGS__, __LINE__,                                                                                        \
        [&](::dansandu::radiance::AssertionResult& dansandu_radiance_internal_assertionResult)                         \
        {                                                                                                              \
            auto dansandu_radiance_internal_result = ::dansandu::radiance::binding::ArgumentBinder{} << __VA_ARGS__ >> \
                                                     ::dansandu::radiance::binding::ArgumentBinder{};                  \
            dansandu_radiance_internal_assertionResult.assertion = std::move(dansandu_radiance_internal_result.first); \
            dansandu_radiance_internal_assertionResult.assertionSuccess = dansandu_radiance_internal_result.second;    \
        }                                                                                                              \
    )

// clang-format off
#if defined(__clang__)
#define REQUIRE(...)                                                                                                   \
    _Pragma("clang diagnostic push")                                                                                   \
    _Pragma("clang diagnostic ignored \"-Woverloaded-shift-op-parentheses\"")                                          \
    DANSANDU_RADIANCE_INTERNAL_ASSERTION(__VA_ARGS__)                                                                  \
    _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
// GCC accepts _Pragma only between statements, so the assertion is wrapped in a block that the trailing semicolon of the
// caller completes. This keeps REQUIRE a single statement (e.g. it can be used in an if statement without braces).
#define REQUIRE(...)                                                                                                   \
    do                                                                                                                 \
    {                                                                                                                  \
        _Pragma("GCC diagnostic push")                                                                                 \
        _Pragma("GCC diagnostic ignored \"-Wparentheses\"")                                                            \
        DANSANDU_RADIANCE_INTERNAL_ASSERTION(__VA_ARGS__);                                                             \
        _Pragma("GCC diagnostic pop")                                                                                  \
    } while (false)
#elif defined(_MSC_VER)
#define REQUIRE(...)                                                                                                   \
    __pragma(warning(push))                                                                                            \
    __pragma(warning(disable : 4554))                                                                                  \
    DANSANDU_RADIANCE_INTERNAL_ASSERTION(__VA_ARGS__)                                                                  \
    __pragma(warning(pop))
#else
#error "Unknown compiler"
#endif

#define DANSANDU_RADIANCE_INTERNAL_THROW_ASSERTION(exception, expression)                                              \
    BaseTestClass::handleThrowAssertion(                                                  \
        #exception ", " #expression, __LINE__, [&]() { expression; }, typeid(exception))

#define DANSANDU_RADIANCE_INTERNAL_LOGGING_ASSERTION(expectedLogs, expression)                                             \
    BaseTestClass::handleLoggingAssertion(                                                           \
        #expectedLogs ", " #expression, __LINE__, expectedLogs, [&]() { expression; })

#if defined(__clang__)
#define REQUIRE_THROW(exception, expression)                                                                           \
    _Pragma("clang diagnostic push")                                                                                   \
    _Pragma("clang diagnostic ignored \"-Wunused-value\"")                                                             \
    DANSANDU_RADIANCE_INTERNAL_THROW_ASSERTION(exception, expression)                                                  \
    _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define REQUIRE_THROW(exception, expression) DANSANDU_RADIANCE_INTERNAL_THROW_ASSERTION(exception, expression)
#elif defined(_MSC_VER)
#define REQUIRE_THROW(exception, expression) DANSANDU_RADIANCE_INTERNAL_THROW_ASSERTION(exception, expression)
#else
#error "Unknown compiler"
#endif

#if defined(__clang__)
#define REQUIRE_LOGGING(expectedLogs, expression)                                                                          \
    _Pragma("clang diagnostic push")                                                                                   \
    _Pragma("clang diagnostic ignored \"-Wunused-value\"")                                                             \
    DANSANDU_RADIANCE_INTERNAL_LOGGING_ASSERTION(expectedLogs, expression)                                                 \
    _Pragma("clang diagnostic pop")
#elif defined(__GNUC__)
#define REQUIRE_LOGGING(expectedLogs, expression) DANSANDU_RADIANCE_INTERNAL_LOGGING_ASSERTION(expectedLogs, expression)
#elif defined(_MSC_VER)
#define REQUIRE_LOGGING(expectedLogs, expression) DANSANDU_RADIANCE_INTERNAL_LOGGING_ASSERTION(expectedLogs, expression)
#else
#error "Unknown compiler"
#endif
// clang-format on

#define TEST_CLASS(className)                                                                                          \
    class PRALINE_EXPORT className                                                                                     \
        : public ::dansandu::radiance::registered_test_class::RegisteredTestClass<className, __FILE__, __LINE__>

#define DANSANDU_RADIANCE_INTERNAL_TEST_METHOD_PREFIX dansandu_radiance_test_method_

#define DANSANDU_RADIANCE_INTERNAL_TEST_METHOD_NAME_2(prefix, line, methodName) prefix##line##_##methodName

#define DANSANDU_RADIANCE_INTERNAL_TEST_METHOD_NAME(prefix, line, methodName)                                          \
    DANSANDU_RADIANCE_INTERNAL_TEST_METHOD_NAME_2(prefix, line, methodName)

#if defined(__clang__)
#define TEST_METHOD(methodName)                                                                                        \
    [[clang::used]] void                                                                                               \
    DANSANDU_RADIANCE_INTERNAL_TEST_METHOD_NAME(DANSANDU_RADIANCE_INTERNAL_TEST_METHOD_PREFIX, __LINE__, methodName)()
#elif defined(__GNUC__)
#define TEST_METHOD(methodName)                                                                                        \
    [[gnu::used]] void                                                                                                 \
    DANSANDU_RADIANCE_INTERNAL_TEST_METHOD_NAME(DANSANDU_RADIANCE_INTERNAL_TEST_METHOD_PREFIX, __LINE__, methodName)()
#elif defined(_MSC_VER)
#define TEST_METHOD(methodName)                                                                                        \
    void                                                                                                               \
    DANSANDU_RADIANCE_INTERNAL_TEST_METHOD_NAME(DANSANDU_RADIANCE_INTERNAL_TEST_METHOD_PREFIX, __LINE__, methodName)()
#else
#error "Unknown compiler"
#endif

namespace dansandu::radiance::macro
{
constexpr auto testMethodPattern =
    DANSANDU_JOURNEY_STRINGIFY(DANSANDU_RADIANCE_INTERNAL_TEST_METHOD_PREFIX) "(\\d+)_(\\w+)\\(\\)$";
}
