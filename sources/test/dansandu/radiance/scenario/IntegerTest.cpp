#include "dansandu/radiance/radiance.hpp"

TEST_CLASS(IntegerTest)
{
public:
    TEST_METHOD(integer_binary_assertion_representation)
    {
        REQUIRE(23 % 10 == 3);
    }

    TEST_METHOD(integer_unary_assertion_representation)
    {
        REQUIRE(myFunction());
    }

private:
    int myFunction()
    {
        return 5;
    }
};
