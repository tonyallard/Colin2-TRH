/*
 * PendingLiteral.cpp
 *
 *  Created on: 8 Jul 2015
 *      Author: tony
 */

#include <sstream>
#include "PendingPNE.h"
#include "Proposition.h"

namespace PDDL {

std::string PendingPNE::toActionString() {
	std::ostringstream output;
	output << "(:durative-action " << (*this) << "\n";
	output << "\t:parameters ()\n";
	output << "\t:duration (= ?duration 1)\n"; //Need to get action duration here
	output << "\t:condition (and \n"; //Need conditions of previous action here
	std::list<std::pair<Proposition, std::pair<VAL::time_spec, bool> > >::const_iterator condItr = conditions.begin();
	const std::list<std::pair<Proposition, std::pair<VAL::time_spec, bool> > >::const_iterator condItrEnd = conditions.end();
	for (; condItr != condItrEnd; condItr++) {
		std::pair<Proposition, std::pair<VAL::time_spec, bool> > cond = *condItr;
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
	output << "\t:effect (\n";
	output << "\t\t";
	output << ((PNE) (*this));
	output << ")\n)";
	return output.str();
}

std::ostream & operator<<(std::ostream & output,
		const PendingPNE & pne) {
	output << pne.getName() << "-";
	std::list<std::string>::const_iterator argItr =
			pne.getArguments().begin();
	const std::list<std::string>::const_iterator argItrEnd =
			pne.getArguments().end();
	for (; argItr != argItrEnd; argItr++) {
		output << (*argItr);
	}
	output << "-" << pne.getValue() << "-" << pne.timestamp;
	return output;
}
}