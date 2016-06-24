#include "demangle.h"

std::string demangle(const char* name) {

	int status = -4;

	char* demangled = abi::__cxa_demangle(name, 0, 0, &status);

	std::string result = (status == 0 ? demangled : name);

	free(demangled);

	return result;
}
