#pragma once

#include "dansandu/journey/exception.hpp"

namespace dansandu::radiance::exception
{

class DuplicateTestCaseNameException : public dansandu::journey::exception::WideException
{
public:
    using WideException::WideException;
};

class TestCaseWithNameNotFoundException : public dansandu::journey::exception::WideException
{
public:
    using WideException::WideException;
};

class AssertionFailureException : public dansandu::journey::exception::WideException
{
public:
    using WideException::WideException;
};

}
