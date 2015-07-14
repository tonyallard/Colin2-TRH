/*
 * PendingAction.cpp
 *
 *  Created on: 14 Jul 2015
 *      Author: tony
 */

#include <sstream>

#include "PendingAction.h"

namespace PDDL {

std::ostream & operator<<(std::ostream & output, const PendingAction & action) {
	output << "(:durative-action " << action.name << "\n";
	output << "\t:parameters ()\n";
	output << "\t:duration (= ?duration " << action.timestamp << ")\n";
	output << "\t:condition (and \n";
	std::list<std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > >::const_iterator condItr =
			action.conditions.begin();
	const std::list<std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > >::const_iterator condItrEnd =
			action.conditions.end();
	for (; condItr != condItrEnd; condItr++) {
		std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > cond =
				*condItr;
		// Check the type of condition
		if (cond.second.first == VAL::time_spec::E_AT_END) {
			output << "\t\t(at end ";
		} else if (cond.second.first == VAL::time_spec::E_OVER_ALL) {
			output << "\t\t(over all ";
		}
		// Check if it is a negative condition
		if (!cond.second.second) {
			output << "(not ";
		}
		// Actually output condition
		output << cond.first;
		// Close negative if required
		if (!cond.second.second) {
			output << ") ";
		}
		output << ")\n";
	}
	output << "\t)\n";
	output << "\t:effect (and\n";
	//first get literal effects
	//Adds
	std::list<PDDL::Literal>::const_iterator litItr = action.literalAddEffects.begin();
	std::list<PDDL::Literal>::const_iterator litItrEnd =
			action.literalAddEffects.end();
	for (; litItr != litItrEnd; litItr++) {
		output << "\t\t(at end " << *litItr << ")\n";
	}
	//Dels
	litItr = action.literalDelEffects.begin();
	litItrEnd = action.literalDelEffects.end();
	for (; litItr != litItrEnd; litItr++) {
		output << "\t\t(at end (not " << *litItr << "))\n";
	}
	//second get pne effects
	std::list<PDDL::PNE>::const_iterator pneItr = action.pneEffects.begin();
	const std::list<PDDL::PNE>::const_iterator pneItrEnd = action.pneEffects.end();
	for (; pneItr != pneItrEnd; pneItr++) {
		output << "\t\t(at end " << *pneItr << ")\n";
	}
	output << "\n\t)\n)\n";
	return output;
}
}

