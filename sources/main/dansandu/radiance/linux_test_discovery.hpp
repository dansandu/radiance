#pragma once

#include "dansandu/radiance/test_class_descriptor.hpp"

#include <map>
#include <set>
#include <string>
#include <vector>

namespace dansandu::radiance::linux_test_discovery
{

std::map<std::string, std::vector<dansandu::radiance::test_class_descriptor::TestMethodDescriptor>>
getLinuxTestClassMethods(const std::string& libraryName, const std::set<std::string>& classNames);

}
