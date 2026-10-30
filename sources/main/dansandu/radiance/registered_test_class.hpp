#pragma once

#include "dansandu/journey/utility.hpp"
#include "dansandu/radiance/base_test_class.hpp"
#include "dansandu/radiance/test_class_registry.hpp"
#include "dansandu/radiance/utility.hpp"

namespace dansandu::radiance::registered_test_class
{

template<const bool*>
struct ForceInstantiation
{
};

template<typename Subclass, dansandu::journey::utility::FixedString FilePath, int LineNumber>
class RegisteredTestClass : public dansandu::radiance::base_test_class::BaseTestClass
{
public:
    static std::unique_ptr<dansandu::radiance::base_test_class::BaseTestClass> produceTestClass()
    {
        return std::unique_ptr<dansandu::radiance::base_test_class::BaseTestClass>{new Subclass{}};
    }

    static void* resolveSubclassAddress(dansandu::radiance::base_test_class::BaseTestClass& testClass)
    {
        return static_cast<Subclass*>(&testClass);
    }

private:
    static const bool registerTestClassResult_;

    // Instantiating the class odr-uses the static member, forcing its definition.
    using ForceRegistration = ForceInstantiation<&registerTestClassResult_>;
};

template<typename Subclass, dansandu::journey::utility::FixedString FilePath, int LineNumber>
const bool RegisteredTestClass<Subclass, FilePath, LineNumber>::registerTestClassResult_ =
    dansandu::radiance::test_class_registry::TestClassRegistry::getGlobalInstance().registerTestClass(
        dansandu::radiance::test_class_registry::TestClassRegistryEntry{
            .testClassName = dansandu::journey::utility::getTypeName<Subclass>(/*includeNamespace*/ true),
            .testClassFilePath = dansandu::radiance::utility::normalizePath(PRALINE_PROJECT_ROOT, FilePath.data),
            .testClassLineNumber = LineNumber,
            .testClassProducer = RegisteredTestClass<Subclass, FilePath, LineNumber>::produceTestClass,
            .subclassAddressResolver = RegisteredTestClass<Subclass, FilePath, LineNumber>::resolveSubclassAddress,
        }
    );

}
