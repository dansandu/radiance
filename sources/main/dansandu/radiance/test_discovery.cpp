#if defined(__linux__)
#include "dansandu/radiance/linux_test_discovery.hpp"
#elif defined(_WIN32)
#include "dansandu/radiance/windows_test_discovery.hpp"
#else
#error "Unknown platform"
#endif

using dansandu::radiance::test_class_descriptor::TestMethodDescriptor;

namespace dansandu::radiance::test_discovery
{

std::map<std::string, std::vector<TestMethodDescriptor>>
getTestClassMethods(const std::string& libraryName, const std::set<std::string>& classNames)
{
#if defined(__linux__)
    return dansandu::radiance::linux_test_discovery::getLinuxTestClassMethods(libraryName, classNames);
#elif defined(_WIN32)
    return dansandu::radiance::windows_test_discovery::getWindowsTestClassMethods(libraryName, classNames);
#else
#error "Unknown platform"
#endif
}

}
