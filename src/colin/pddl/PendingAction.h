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

#include "Literal.h"
#include "PNE.h"
#include "ptree.h"

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
	std::list<Literal> literalAddEffects;
	std::list<Literal> literalDelEffects;
	std::list<PNE> pneEffects;
	std::list<std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions;

public:
	PendingAction(std::string name, std::list<Literal> literalAddEffects, std::list<Literal> literalDelEffects,
			std::list<PNE> pneEffects,
			std::list<std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions,
			double timestamp) :
			name(name), literalAddEffects(literalAddEffects), literalDelEffects(literalDelEffects), pneEffects(pneEffects), conditions(
					conditions), timestamp(timestamp) {
	}
	;
	const std::string & getName() const { return name; };
	friend std::ostream & operator<<(std::ostream & output,
			const PendingAction & action);
};

}

#endif /* COLIN_PDDL_PENDINGACTION_H_ */
