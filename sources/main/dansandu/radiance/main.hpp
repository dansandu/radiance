#pragma once
#ifdef DANSANDU_RADIANCE_SUPPLY_DEFAULT_MAIN
#undef DANSANDU_RADIANCE_SUPPLY_DEFAULT_MAIN
#include "dansandu/radiance/test_case_registry.hpp"

using dansandu::radiance::exception::wrapInTryCatch;
using dansandu::radiance::test_case_registry::runTestSuite;

int main(const int argumentCount, const char* const* const arguments)
{
    return wrapInTryCatch(runTestSuite, argumentCount, arguments);
}
#endif
