/*
 * PendingAction.cpp
 *
 *  Created on: 14 Jul 2015
 *      Author: tony
 */

#include <sstream>
#include <string>

#include "PendingAction.h"

using namespace std;
namespace PDDL {

map<const PDDLObject *, string> PendingAction::generateParameterTable() const {
	map<const PDDLObject *, string> parameterTable;
	//FIXME: means that there can only be 24 parameters before we create errors
	char letter = 'a';
	std::set<PDDLObject>::const_iterator paramItr = parameters.begin();
	for (; paramItr != parameters.end(); paramItr++) {
		const PDDLObject * pddlObj = &(*paramItr);
		ostringstream paramVar;
		paramVar << "?" << static_cast<char>(letter);
		letter++;
		parameterTable.insert(
				pair<const PDDLObject *, string>(pddlObj, paramVar.str()));
	}
	return parameterTable;
}

std::ostream & operator<<(std::ostream & output, const PendingAction & action) {
	map<const PDDLObject *, string> parameterTable =
			action.generateParameterTable();
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
		output << "\t\t(at end " << pneItr->toActionEffectString(parameterTable)
				<< ")\n";
	}
	output << "\n\t\t)\n\t)\n";
	return output;
}
}

