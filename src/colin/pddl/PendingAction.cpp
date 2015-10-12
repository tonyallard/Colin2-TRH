/*
 * PendingAction.cpp
 *
 *  Created on: 14 Jul 2015
 *      Author: tony
 */

#include <sstream>

#include "PendingAction.h"

namespace PDDL {

bool PendingAction::operator==(const PendingAction & other) {
	//check if the name is the same
	if (this->name != other.name) {
		return false;
	}
	//check if timestamp is the same
	if (this->timestamp != other.timestamp) {
		return false;
	}
	//check if the prop adds are the same
	if (this->literalAddEffects.size() != other.literalAddEffects.size()) {
		return false;
	}
	std::list<Proposition>::const_iterator thisAddItr =
			this->literalAddEffects.begin();
	std::list<Proposition>::const_iterator othrAddItr =
			other.literalAddEffects.begin();
	for (; thisAddItr != this->literalAddEffects.end();
			thisAddItr++, othrAddItr++) {
		Proposition thisAdd = *thisAddItr;
		Proposition othrAdd = *othrAddItr;
		if (thisAdd != othrAdd) {
			return false;
		}
	}
	//check if the prop dels are the same
	if (this->literalDelEffects.size() != other.literalDelEffects.size()) {
		return false;
	}
	std::list<Proposition>::const_iterator thisDelItr =
			this->literalDelEffects.begin();
	std::list<Proposition>::const_iterator othrDelItr =
			other.literalDelEffects.begin();
	for (; thisDelItr != this->literalDelEffects.end();
			thisDelItr++, othrDelItr++) {
		Proposition thisDel = *thisDelItr;
		Proposition othrDel = *othrDelItr;
		if (thisDel != othrDel) {
			return false;
		}
	}
	//check if the PNE effects are the same
	if (this->pneEffects.size() != other.pneEffects.size()) {
		return false;
	}
	std::list<PNE>::const_iterator thisPNEItr =
			this->pneEffects.begin();
	std::list<PNE>::const_iterator othrPNEItr =
			other.pneEffects.begin();
	for (; thisPNEItr != this->pneEffects.end(); thisPNEItr++, othrPNEItr++) {
		PNE thisPNE = *thisPNEItr;
		PNE othrPNE = *othrPNEItr;
		if (thisPNE != othrPNE) {
			return false;
		}
	}
	//check if preconditions are the same
	if (this->conditions.size() != other.conditions.size()) {
		return false;
	}
	std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > >::const_iterator thisCondItr =
			this->conditions.begin();
	std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > >::const_iterator othrCondItr =
			other.conditions.begin();
	for (; thisCondItr != this->conditions.end();
			thisCondItr++, othrCondItr++) {
		std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > thisCond =
				*thisCondItr;
		std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > othrCond =
				*othrCondItr;
		if (thisCond.first != othrCond.first) {
			return false;
		}
		if (thisCond.second.first != othrCond.second.first) {
			return false;
		}
		if (thisCond.second.second != othrCond.second.second) {
			return false;
		}
	}
	//They are the same
	return true;
}

bool PendingAction::operator!=(const PendingAction & other) {
	return !((*this) == other);
}

std::ostream & operator<<(std::ostream & output, const PendingAction & action) {
	output << "(:durative-action " << action.name << "\n";
	output << "\t:parameters ()\n";
	output << "\t:duration (= ?duration " << action.timestamp << ")\n";
	output << "\t:condition (and \n";
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
	std::list<PDDL::Proposition>::const_iterator litItr =
			action.literalAddEffects.begin();
	std::list<PDDL::Proposition>::const_iterator litItrEnd =
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
	const std::list<PDDL::PNE>::const_iterator pneItrEnd =
			action.pneEffects.end();
	for (; pneItr != pneItrEnd; pneItr++) {
		output << "\t\t(at end " << pneItr->toActionEffectString() << ")\n";
	}
	output << "\n\t)\n)\n";
	return output;
}
}

