#include <util/foreach.h>
#include "LinearConstraint.h"

LinearConstraint::LinearConstraint() :
	_relation(LessEqual) {}

void
LinearConstraint::setCoefficient(unsigned int varNum, double coef) {

	if (coef == 0) {

		std::map<unsigned int, double>::iterator i = _coefs.find(varNum);
		if (i != _coefs.end())
			_coefs.erase(_coefs.find(varNum));

	} else {

		_coefs[varNum] = coef;
	}
}

void
LinearConstraint::setRelation(Relation relation) {

	_relation = relation;
}

bool LinearConstraint::isViolated(const Solution & solution){

    double s = 0;

    for(const auto & kv : _coefs){
        const auto var = kv.first;
        const auto coef = kv.second;
        const auto sol = solution[var];
        s+= coef*sol;
    }
    if(_relation == LessEqual){
        return s > _value;
    }
    else if(_relation == GreaterEqual){
        return s < _value;
    }
    else{
        return s != _value;
    }
}


void
LinearConstraint::setValue(double value) {

	_value = value;
}

const std::map<unsigned int, double>&
LinearConstraint::getCoefficients() const {

	return _coefs;
}

const Relation&
LinearConstraint::getRelation() const {

	return _relation;
}

double
LinearConstraint::getValue() const {

	return _value;
}

std::ostream& operator<<(std::ostream& out, const LinearConstraint& constraint) {

	typedef std::map<unsigned int, double>::value_type pair_t;
	foreach (const pair_t& pair, constraint.getCoefficients())
		out << pair.second << "*" << pair.first << " ";

	out << (constraint.getRelation() == LessEqual ? "<=" : (constraint.getRelation() == GreaterEqual ? ">=" : "=="));

	out << " " << constraint.getValue();

	return out;
}
