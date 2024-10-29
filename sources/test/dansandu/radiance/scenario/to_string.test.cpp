#include "dansandu/radiance/radiance.hpp"

#include <string>

namespace
{

class MyCustomType
{
public:
    MyCustomType(const int id) : id_{id}
    {
    }

    std::string toString() const
    {
        return "MyCustomType(" + std::to_string(id_) + ")";
    }

private:
    int id_;
};

bool operator==(const MyCustomType&, const MyCustomType)
{
    return true;
}

}

TEST_CASE("to_string")
{
    REQUIRE(MyCustomType(1) == MyCustomType(2));
}
