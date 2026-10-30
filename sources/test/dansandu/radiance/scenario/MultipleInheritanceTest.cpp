#include "dansandu/radiance/radiance.hpp"

#include <string>

class PRALINE_EXPORT Fixture
{
public:
    Fixture() : fixtureName_{"fixture"}
    {
    }

    virtual ~Fixture() noexcept = default;

protected:
    std::string fixtureName_;
};

TEST_CLASS(MultipleInheritanceTest)
    , public Fixture
    {
    public:
        MultipleInheritanceTest() : testName_{"test"}
        {
        }

        TEST_METHOD(members_of_all_bases_are_accessible)
        {
            REQUIRE(fixtureName_ == "fixture");

            REQUIRE(testName_ == "test");
        }

    private:
        std::string testName_;
    };
