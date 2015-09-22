/*
 * PendingLiteral.h
 *
 *  Created on: 8 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PDDL_PENDINGLITERAL_H_
#define COLIN_PDDL_PENDINGLITERAL_H_

#include <string>
#include <iostream>
#include <list>

#include "Literal.h"
#include "ptree.h"

namespace PDDL {

class PendingProposition: public PDDL::Proposition {
private:
	double timestamp;
	std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions;
	bool addEffect;

public:
	PendingProposition(std::string name, std::list<std::string> arguments,
			std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions, double timestamp, bool addEffect) :
			PDDL::Proposition(name, arguments), conditions(conditions), timestamp(timestamp), addEffect(addEffect) {
	};
	std::string toTILString();
	std::string toActionString();
	friend std::ostream & operator<<(std::ostream & output,
			const PendingProposition & literal);
};

}

#endif /* COLIN_PDDL_PENDINGLITERAL_H_ */
