/*
 * PendingPNE.h
 *
 *  Created on: 8 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PDDL_PENDINGPNE_H_
#define COLIN_PDDL_PENDINGPNE_H_

#include <string>
#include <iostream>
#include <list>

#include "Literal.h"
#include "PNE.h"
#include "ptree.h"

namespace PDDL {

class PendingPNE: public PDDL::PNE {
private:
	double timestamp;
	std::list<std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions;

public:
	PendingPNE(std::string name, std::list<std::string> arguments, double value,
			std::list<std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions, double timestamp) :
			PDDL::PNE(name, arguments, value), conditions(conditions), timestamp(timestamp) {
	};
	std::string toActionString();
	friend std::ostream & operator<<(std::ostream & output,
			const PendingPNE & pne);
};

}

#endif /* COLIN_PDDL_PENDINGPNE_H_ */
