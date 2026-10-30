#pragma once

#include <string>
#include <vector>

namespace dansandu::radiance::library
{

struct LibrarySymbol
{
    std::string name;
    void* pointer;
};

std::vector<LibrarySymbol> getClassMethods(const std::string& libraryName, const std::string& className,
                                           const std::string& methodPrefix);

// Only use on non-virtual class methods with signature void(Class::*)().
void invokeVoidClassMethod(const LibrarySymbol& symbol, void* const object);

}
