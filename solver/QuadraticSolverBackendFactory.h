#ifndef QUADRATIC_PROGRAM_SOLVER_FACTORY_H__
#define QUADRATIC_PROGRAM_SOLVER_FACTORY_H__

#include "QuadraticSolverBackend.h"
#include "BackendPreference.h"

class QuadraticSolverBackendFactory {

public:

	virtual QuadraticSolverBackend* createQuadraticSolverBackend(Preference preference = Any) const = 0;
};

#endif // QUADRATIC_PROGRAM_SOLVER_FACTORY_H__

