#ifndef UTIL_DEMANGLE_H__
#define UTIL_DEMANGLE_H__

#include <cxxabi.h>
#include <cstdlib>
#include <string>

std::string demangle(const char* name);

#endif // UTIL_DEMANGLE_H__

