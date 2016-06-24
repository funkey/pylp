#ifndef TYPENAME_H__
#define TYPENAME_H__

#include <typeinfo>
#include "demangle.h"

namespace {

template <typename T>
std::string typeName(T* p) {

	return demangle(typeid(*p).name());
}

template <typename T>
std::string typeName(const T& p) {

	return demangle(typeid(p).name());
}

}

#endif // TYPENAME_H__

