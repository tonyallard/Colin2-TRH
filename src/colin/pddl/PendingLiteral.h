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

class PendingLiteral: public PDDL::Literal {
private:
	double timestamp;
	std::list<std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions;
	bool addEffect;

public:
	PendingLiteral(std::string name, std::list<std::string> arguments,
			std::list<std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions, double timestamp, bool addEffect) :
			PDDL::Literal(name, arguments), conditions(conditions), timestamp(timestamp), addEffect(addEffect) {
	};
	std::string toTILString();
	std::string toActionString();
	friend std::ostream & operator<<(std::ostream & output,
			const PendingLiteral & literal);
};

}

#endif /* COLIN_PDDL_PENDINGLITERAL_H_ */
