/*
 * PendingAction.cpp
 *
 *  Created on: 14 Jul 2015
 *      Author: tony
 */

#include <sstream>
#include <string>

#include "PendingAction.h"
#include "PDDLUtils.h"
#include "MMCRDomainFactory.h"

using namespace std;
namespace PDDL {

std::ostream & operator<<(std::ostream & output, const PendingAction & action) {
	map<const PDDLObject *, string> parameterTable =
			PDDL::generateParameterTable(action.parameters);
	output << "\t(:durative-action " << action.name << "\n";
	output << "\t\t:parameters (";
	map<const PDDLObject *, string>::const_iterator paramItr =
			parameterTable.begin();
	for (; paramItr != parameterTable.end(); paramItr++) {
		output << paramItr->second << " - " << paramItr->first->getType()
				<< " ";
	}
	output << ")\n";
	output << "\t\t:duration (= ?duration " << action.timestamp << ")\n";
	output << "\t\t:condition (and \n";
	//Add pre-condition on initial action
	output << "\t\t\t(at start (initial-action-complete))\n";
	//Add pre-conditions for required parameters
	paramItr = parameterTable.begin();
	for (; paramItr != parameterTable.end(); paramItr++) {
		output << "\t\t\t(at start (" << MMCRDomainFactory::REQUIRED_PROPOSITION
				<< " " << paramItr->second << "))" << std::endl;
	}
	std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > >::const_iterator condItr =
			action.conditions.begin();
	const std::list<
			std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > >::const_iterator condItrEnd =
			action.conditions.end();
	for (; condItr != condItrEnd; condItr++) {
		std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > cond =
				*condItr;
		// Check the type of condition
		if (cond.second.first == VAL::time_spec::E_AT_END) {
			output << "\t\t\t(at end ";
		} else if (cond.second.first == VAL::time_spec::E_OVER_ALL) {
			output << "\t\t\t(over all ";
		}
		// Check if it is a negative condition
		if (!cond.second.second) {
			output << "(not ";
		}
		// Actually output condition
		output << cond.first.toParameterisedString(parameterTable);
		// Close negative if required
		if (!cond.second.second) {
			output << ") ";
		}
		output << ")\n";
	}
	output << "\t\t)\n";
	output << "\t\t:effect (and\n";
	//Add effects for required parameters
	paramItr = parameterTable.begin();
	for (; paramItr != parameterTable.end(); paramItr++) {
		output << "\t\t\t(at start (not (" << MMCRDomainFactory::REQUIRED_PROPOSITION
				<< " " << paramItr->second << ")))" << std::endl;
	}
	//first get propositional effects
	//Adds
	std::list<PDDL::Proposition>::const_iterator litItr =
			action.literalAddEffects.begin();
	std::list<PDDL::Proposition>::const_iterator litItrEnd =
			action.literalAddEffects.end();
	for (; litItr != litItrEnd; litItr++) {
		output << "\t\t\t(at end "
				<< litItr->toParameterisedString(parameterTable) << ")\n";
	}
	//Dels
	litItr = action.literalDelEffects.begin();
	litItrEnd = action.literalDelEffects.end();
	for (; litItr != litItrEnd; litItr++) {
		output << "\t\t\t(at end (not "
				<< litItr->toParameterisedString(parameterTable) << "))\n";
	}
	//second get pne effects
	std::list<PDDL::PNE>::const_iterator pneItr = action.pneEffects.begin();
	const std::list<PDDL::PNE>::const_iterator pneItrEnd =
			action.pneEffects.end();
	for (; pneItr != pneItrEnd; pneItr++) {
		output << "\t\t\t(at end " << pneItr->toActionEffectString(parameterTable)
				<< ")\n";
	}
	output << "\n\t\t)\n\t)\n";
	return output;
}
}

