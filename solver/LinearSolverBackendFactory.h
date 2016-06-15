#ifndef LINEAR_PROGRAM_SOLVER_FACTORY_H__
#define LINEAR_PROGRAM_SOLVER_FACTORY_H__

#include "BackendPreference.h"

// forward declaration
class LinearSolverBackend;

class LinearSolverBackendFactory {

public:

	virtual LinearSolverBackend* createLinearSolverBackend(Preference preference = Any) const = 0;
};

#endif // LINEAR_PROGRAM_SOLVER_FACTORY_H__

