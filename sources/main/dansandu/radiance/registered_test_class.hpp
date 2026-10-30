#pragma once

#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/base_test_class.hpp"
#include "dansandu/radiance/test_class_registry.hpp"

namespace dansandu::radiance::registered_test_class
{

template<typename Subclass, dansandu::journey::utility::FixedString FilePath, int LineNumber>
class RegisteredTestClass : public dansandu::radiance::base_test_class::BaseTestClass
{
public:
    RegisteredTestClass()
    {
        // Force registerTestClassResult_ instantiation.
        static_cast<void>(registerTestClassResult_);
    }

private:
    static const bool registerTestClassResult_;
};

template<typename Subclass, dansandu::journey::utility::FixedString FilePath, int LineNumber>
const bool RegisteredTestClass<Subclass, FilePath, LineNumber>::registerTestClassResult_ =
    dansandu::radiance::test_class_registry::TestClassRegistry::getGlobalInstance().registerTestClass(
        dansandu::radiance::test_class_registry::TestClassRegistryEntry{
            .testClassName = dansandu::journey::utility::getTypeName<Subclass>(/*includeNamespace*/ true),
            .testClassFilePath = FilePath.data,
            .testClassLineNumber = LineNumber,
            .testClassProducer = []()
            {
                return std::unique_ptr<dansandu::radiance::base_test_class::BaseTestClass>{new Subclass{}};
            }});

}
