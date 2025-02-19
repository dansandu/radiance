#pragma once

#include <functional>
#include <stdexcept>
#include <string>

namespace dansandu::radiance::exception
{

class BaseRadianceException : public std::exception
{
public:
    BaseRadianceException()
    {
    }

    explicit BaseRadianceException(const std::wstring& message) : message_{message}
    {
    }

    virtual const std::wstring& message() const noexcept
    {
        return message_;
    }

    const char* what() const noexcept override
    {
        return "This is a wide string exception -- see message() for actual message";
    }

private:
    std::wstring message_;
};

class DuplicateTestCaseNameException : public BaseRadianceException
{
public:
    using BaseRadianceException::BaseRadianceException;
};

class TestCaseWithNameNotFoundException : public BaseRadianceException
{
public:
    using BaseRadianceException::BaseRadianceException;
};

PRALINE_EXPORT int wrapInTryCatch(const std::function<int(const int, const char* const* const)>& callback,
                                  const int argumentCount, const char* const* const arguments);

}
