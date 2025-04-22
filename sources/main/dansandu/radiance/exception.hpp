#pragma once

#include "dansandu/journey/exception.hpp"

namespace dansandu::radiance::exception
{

class DuplicateTestCaseNameException : public dansandu::journey::exception::Exception
{
public:
    using Exception::Exception;
};

class TestCaseWithNameNotFoundException : public dansandu::journey::exception::Exception
{
public:
    using Exception::Exception;
};

}
