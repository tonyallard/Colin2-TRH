/*
 * PendingAction.h
 *
 *  Created on: 14 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PDDL_PENDINGACTION_H_
#define COLIN_PDDL_PENDINGACTION_H_

#include <string>
#include <iostream>
#include <list>
#include <set>
#include <map>

#include "PDDLObject.h"
#include "Proposition.h"
#include "PNE.h"
#include "ptree.h"

using namespace std;

namespace PDDL {

/**
 * This class models actions that have started but not yet complete.
 * Here we assume that start conditions have already been satisfied
 * and start effects have already been applied previously. Therefore
 * we do not need to account for them in this action and can
 * significantly simply the syntax.
 */
class PendingAction {
private:
	double timestamp;
	std::string name;
	std::set<PDDLObject> parameters;
	std::list<Proposition> literalAddEffects;
	std::list<Proposition> literalDelEffects;
	std::list<PNE> pneEffects;
	std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions;

	map<const PDDLObject *, string> generateParameterTable() const;

public:
	PendingAction(std::string name, std::set<PDDLObject> parameters, std::list<Proposition> literalAddEffects, std::list<Proposition> literalDelEffects,
			std::list<PNE> pneEffects,
			std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions,
			double timestamp) :
			name(name), parameters(parameters), literalAddEffects(literalAddEffects), literalDelEffects(literalDelEffects), pneEffects(pneEffects), conditions(
					conditions), timestamp(timestamp) {
	}
	;
	const std::string & getName() const { return name; };
	const std::set<PDDLObject> & getParameters() const {return parameters; };
	friend std::ostream & operator<<(std::ostream & output,
			const PendingAction & action);
};

}

#endif /* COLIN_PDDL_PENDINGACTION_H_ */
