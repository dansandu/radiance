#include "dansandu/radiance/test_class_registry.hpp"
#include "dansandu/journey/exception.hpp"
#include "dansandu/journey/logging.hpp"
#include "dansandu/radiance/common.hpp"
#include "dansandu/radiance/test_class_descriptor.hpp"
#include "dansandu/radiance/test_discovery.hpp"

#include <map>
#include <mutex>
#include <set>
#include <vector>

using dansandu::radiance::test_class_descriptor::TestClassDescriptor;
using dansandu::radiance::test_class_descriptor::TestMethodDescriptor;
using dansandu::radiance::test_discovery::getTestClassMethods;

namespace dansandu::radiance::test_class_registry
{

TestClassRegistry& TestClassRegistry::getGlobalInstance()
{
    static auto globalTestClassRegistry = TestClassRegistry{};

    return globalTestClassRegistry;
}

bool TestClassRegistry::registerTestClass(TestClassRegistryEntry&& testClassRegistryEntry)
{
    const auto lock = std::lock_guard{mutex_};

    testClassRegistry_.push_back(std::move(testClassRegistryEntry));

    return true;
}

std::map<std::string, TestClassDescriptor> TestClassRegistry::getTestClassDescriptors(const std::string& libraryName)
{
    auto testClassRegistryCopy = std::vector<TestClassRegistryEntry>{};

    {
        const auto lock = std::lock_guard{mutex_};

        testClassRegistryCopy = testClassRegistry_;
    }

    auto classNames = std::set<std::string>{};

    for (const auto& entry : testClassRegistryCopy)
    {
        classNames.insert(entry.testClassName);
    }

    if (testClassRegistryCopy.empty())
    {
        LOG_WARNING("No test classes were registered");
    }

    const auto testMethodsByClassName = getTestClassMethods(libraryName, classNames);

    auto testClassesByClassName = std::map<std::string, TestClassDescriptor>{};

    for (const auto& entry : testClassRegistryCopy)
    {
        const auto position = testMethodsByClassName.find(entry.testClassName);

        if (position == testMethodsByClassName.cend())
        {
            LOG_WARNING("No test methods were found for test class '", entry.testClassName, "'");

            continue;
        }

        const auto [iterator, inserted] = testClassesByClassName.try_emplace(
            entry.testClassName, TestClassDescriptor{
                                     entry.testClassName, entry.testClassFilePath, entry.testClassLineNumber,
                                     entry.testClassProducer, entry.subclassAddressResolver, position->second
                                 }
        );

        if (!inserted)
        {
            THROW(std::logic_error, "Duplicate test class '", entry.testClassName, "'");
        }
    }

    return testClassesByClassName;
}

}
