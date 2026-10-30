#include "dansandu/radiance/radiance.hpp"

TEST_CLASS(StringTest)
{
public:
    TEST_METHOD(string_representation)
    {
        const auto string = std::string{"string"};

        REQUIRE(string == string);

        REQUIRE(!string.empty());
    }

    TEST_METHOD(string_representation_escapes_special_characters)
    {
        REQUIRE(std::string{"\x1B[32mGreen\x1B[0m\n\"text\" \\\t"} != "string");
    }
};
