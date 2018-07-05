#include <sstream>
#include <boost/python.hpp>
#include <boost/python/exception_translator.hpp>
#include <boost/python/suite/indexing/vector_indexing_suite.hpp>
#include <boost/python/suite/indexing/map_indexing_suite.hpp>

#include <util/exceptions.h>
#include <solvers/BackendPreference.h>
#include <solvers/SolverFactory.h>
#include <solvers/ScipBackend.h>
#include <solvers/GurobiBackend.h>
#include <solvers/CplexBackend.h>
#include "logging.h"
#include "config.h"

template <typename Map, typename K, typename V>
const V& genericGetter(const Map& map, const K& k) { return map[k]; }
template <typename Map, typename K, typename V>
void genericSetter(Map& map, const K& k, const V& value) { map[k] = value; }


template <typename T>
std::string print(const T& t) {

	std::stringstream ss;
	ss << t;
	return ss.str();
}

#if defined __clang__ && __clang_major__ < 6
// std::shared_ptr support
	template<class T> T* get_pointer(std::shared_ptr<T> p){ return p.get(); }
#endif

namespace pylp {

/**
 * Translates an Exception into a python exception.
 *
 **/
void translateException(const Exception& e) {

	if (boost::get_error_info<error_message>(e))
		PyErr_SetString(PyExc_RuntimeError, boost::get_error_info<error_message>(e)->c_str());
	else
		PyErr_SetString(PyExc_RuntimeError, e.what());
}

/**
 * Defines all the python classes in the module libpylp. Here we decide 
 * which functions and data members we wish to expose.
 */
BOOST_PYTHON_MODULE(pylp) {

	boost::python::register_exception_translator<Exception>(&translateException);

	// Logging
	boost::python::enum_<logger::LogLevel>("LogLevel")
			.value("Quiet", logger::Quiet)
			.value("Error", logger::Error)
			.value("Debug", logger::Debug)
			.value("All", logger::All)
			.value("User", logger::User)
			;
	boost::python::def("setLogLevel", setLogLevel);
	boost::python::def("getLogLevel", getLogLevel);

	boost::python::enum_<VariableType>("VariableType")
			.value("Continuous" ,Continuous)
			.value("Integer", Integer)
			.value("Binary", Binary)
			;

	// std::vector<double>
	boost::python::class_<std::vector<double>>("vector_d")
		.def(boost::python::init<>())
		.def(boost::python::init<std::size_t>())
		.def(boost::python::vector_indexing_suite<std::vector<double>>())
	;

	// std::map<unsigned int, double>
	boost::python::class_<std::map<unsigned int, double>>("map_uid")
		.def(boost::python::init<>())
		.def(boost::python::map_indexing_suite<std::map<unsigned int, double>>())
	;

	// std::map<unsigned int, VariableType>
	boost::python::class_<std::map<unsigned int, VariableType>>("VariableTypeMap")
		.def(boost::python::init<>())
		.def(boost::python::map_indexing_suite<std::map<unsigned int, VariableType>>())
	;

	// Sense
	boost::python::enum_<Sense>("Sense")
			.value("Minimize", Minimize)
			.value("Maximize", Maximize)
			;

	// Relation
	boost::python::enum_<Relation>("Relation")
			.value("LessEqual", LessEqual)
			.value("Equal", Equal)
			.value("GreaterEqual", GreaterEqual)
			;

	// Solution
	boost::python::class_<Solution>("Solution", boost::python::init<>())
			.def("__getitem__", &genericGetter<Solution, unsigned int, double>, boost::python::return_value_policy<boost::python::copy_const_reference>())
			.def("__setitem__", &genericSetter<Solution, unsigned int, double>)
			.def("get_vector", &Solution::getVector, boost::python::return_value_policy<boost::python::copy_const_reference>())
			.def("set_value", &Solution::setValue)
			.def("get_value", &Solution::getValue)
			.def("__len__", &Solution::size)
	;

	// Objective
	boost::python::class_<LinearObjective>("LinearObjective", boost::python::init<unsigned int>())
			.def("set_constant", &LinearObjective::setConstant)
			.def("get_constant", &LinearObjective::getConstant)
			.def("set_coefficient", &LinearObjective::setCoefficient)
			.def("get_coefficients", &LinearObjective::getCoefficients, boost::python::return_value_policy<boost::python::copy_const_reference>())
			.def("set_sense", &LinearObjective::setSense)
			.def("get_sense", &LinearObjective::getSense)
			.def("resize", &LinearObjective::resize)
			.def("__len__", &LinearObjective::size)
	;

	// Constraint
	boost::python::class_<LinearConstraint>("LinearConstraint")
			.def("set_coefficient", &LinearConstraint::setCoefficient)
			.def("set_relation", &LinearConstraint::setRelation)
			.def("set_value", &LinearConstraint::setValue)
			.def("get_coefficients", &LinearConstraint::getCoefficients, boost::python::return_value_policy<boost::python::copy_const_reference>())
			.def("get_relation", &LinearConstraint::getRelation, boost::python::return_value_policy<boost::python::copy_const_reference>())
			.def("get_value", &LinearConstraint::getValue)
			.def("is_violated", &LinearConstraint::isViolated)
			.def("__str__", &print<LinearConstraint>)
	;

	// Constraints
	boost::python::class_<LinearConstraints>("LinearConstraints", boost::python::init<>())
			.def("clear", &LinearConstraints::clear)
			.def("add", &LinearConstraints::add)
			.def("add_all", &LinearConstraints::addAll)
			.def("__len__", &LinearConstraints::size)
			.def("__getitem__", &genericGetter<LinearConstraints, size_t, const LinearConstraint&>, boost::python::return_value_policy<boost::python::copy_const_reference>())
			.def("__setitem__", &genericSetter<LinearConstraints, size_t, const LinearConstraint&>)
	;

	// ScipBackend
	boost::python::class_<ScipBackend, boost::noncopyable>("ScipBackend")
			.def("initialize", static_cast<void(ScipBackend::*)(unsigned int, VariableType)>(&ScipBackend::initialize))
			.def("initialize", static_cast<void(ScipBackend::*)(unsigned int, VariableType, const std::map<unsigned int, VariableType>&)>(&ScipBackend::initialize))
			.def("set_objective", static_cast<void(ScipBackend::*)(const LinearObjective&)>(&ScipBackend::setObjective))
			.def("set_constraints", &ScipBackend::setConstraints)
			.def("solve", static_cast<std::string(ScipBackend::*)(Solution&)>(&ScipBackend::solve))
			;

#ifdef HAVE_GUROBI
	// GurobiBackend
	boost::python::class_<GurobiBackend, boost::noncopyable>("GurobiBackend")
			.def("initialize", static_cast<void(GurobiBackend::*)(unsigned int, VariableType)>(&GurobiBackend::initialize))
			.def("initialize", static_cast<void(GurobiBackend::*)(unsigned int, VariableType, const std::map<unsigned int, VariableType>&)>(&GurobiBackend::initialize))
			.def("set_objective", static_cast<void(GurobiBackend::*)(const LinearObjective&)>(&GurobiBackend::setObjective))
			.def("set_constraints", &GurobiBackend::setConstraints)
			.def("solve", static_cast<std::string(GurobiBackend::*)(Solution&)>(&GurobiBackend::solve))
			;
#endif

#ifdef HAVE_CPLEX
	// CplexBackend
	boost::python::class_<CplexBackend, boost::noncopyable>("CplexBackend")
			.def("initialize", static_cast<void(CplexBackend::*)(unsigned int, VariableType)>(&CplexBackend::initialize))
			.def("initialize", static_cast<void(CplexBackend::*)(unsigned int, VariableType, const std::map<unsigned int, VariableType>&)>(&CplexBackend::initialize))
			.def("set_objective", static_cast<void(CplexBackend::*)(const LinearObjective&)>(&CplexBackend::setObjective))
			.def("set_constraints", &CplexBackend::setConstraints)
			.def("solve", static_cast<std::string(CplexBackend::*)(Solution&)>(&CplexBackend::solve))
			;
#endif
}

} // namespace pylp
