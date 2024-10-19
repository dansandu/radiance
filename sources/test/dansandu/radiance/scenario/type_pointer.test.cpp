#include "dansandu/radiance/radiance.hpp"

TEST_CASE(L"type_pointer")
{
    const auto myVoidPointer = reinterpret_cast<const void*>(1234567);

    REQUIRE(myVoidPointer != nullptr);
}
