#include "SolverFactory.h"

#include <config.h>
#include <util/ProgramOptions.h>

#ifdef HAVE_GUROBI
#include "GurobiBackend.h"
#endif

#ifdef HAVE_CPLEX
#include "CplexBackend.h"
#endif

#ifdef HAVE_SCIP
#include "ScipBackend.h"
#endif

util::ProgramOption optionUseGurobi(
		util::_long_name        = "useGurobi",
		util::_description_text = "Use the gurobi solver for ILPs and QPs. If not set, the first "
		                          "available solver will be used."
);

util::ProgramOption optionUseCplex(
		util::_long_name        = "useCplex",
		util::_description_text = "Use the CPLEX solver for ILPs and QPs. If not set, the first "
		                          "available solver will be used."
);

util::ProgramOption optionUseScip(
		util::_long_name        = "useScip",
		util::_description_text = "Use the SCIP solver for ILPs and QPs. If not set, the first "
		                          "available solver will be used."
);

LinearSolverBackend*
SolverFactory::createLinearSolverBackend(Preference preference) const {

	if (optionUseGurobi.as<bool>() && optionUseCplex.as<bool>())
		UTIL_THROW_EXCEPTION(
				LinearSolverBackendException,
				"only one solver can be chosen");

	// use program options, if we were not forced to use a particular solver 
	// already
	if (preference == Any) {

		if (optionUseGurobi)
			preference = Gurobi;
		if (optionUseCplex)
			preference = Cplex;
		if (optionUseScip)
			preference = Scip;
	}

// by default, create a gurobi backend
#ifdef HAVE_GUROBI

	if (preference == Any || preference == Gurobi)
		return new GurobiBackend();

#endif

// if this is not available, create a CPLEX backend
#ifdef HAVE_CPLEX

	if (preference == Any || preference == Cplex)
		return new CplexBackend();

#endif

// if this is not available, create a SCIP backend
#ifdef HAVE_SCIP

	if (preference == Any || preference == Scip)
		return new ScipBackend();

#endif

// if this is not available as well, throw an exception

	BOOST_THROW_EXCEPTION(NoSolverException() << error_message("No linear solver available."));
}

QuadraticSolverBackend*
SolverFactory::createQuadraticSolverBackend(Preference preference) const {

	if (optionUseGurobi.as<bool>() && optionUseCplex.as<bool>())
		UTIL_THROW_EXCEPTION(
				LinearSolverBackendException,
				"only one solver can be chosen");

	// use program options, if we were not forced to use a particular solver 
	// already
	if (preference == Any) {

		if (optionUseGurobi)
			preference = Gurobi;
		if (optionUseCplex)
			preference = Cplex;
	}

// by default, create a gurobi backend
#ifdef HAVE_GUROBI

	if (preference == Any || preference == Gurobi)
			return new GurobiBackend();

#endif

// if this is not available, create a CPLEX backend
#ifdef HAVE_CPLEX

	if (preference == Any || preference == Cplex)
		return new CplexBackend();

#endif

// if this is not available as well, throw an exception

	BOOST_THROW_EXCEPTION(NoSolverException() << error_message("No quadratic solver available."));
}
