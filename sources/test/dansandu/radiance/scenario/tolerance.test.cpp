#include "dansandu/radiance/radiance.hpp"

using dansandu::radiance::Tolerance;

TEST_CASE("tolerance")
{
    REQUIRE(0.1 == Tolerance(0.1));

    REQUIRE(1000.0 == Tolerance(1001.0, 1.0e-3));

    REQUIRE(0.0001 == Tolerance(0.0005, 0.0, 0.001));

    REQUIRE(1000.0 != Tolerance(1100.0, 0.01));

    REQUIRE(0.0001 != Tolerance(0.01, 0.0, 0.001));
}
