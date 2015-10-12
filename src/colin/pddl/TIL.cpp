/*
 * TIL.cpp
 *
 *  Created on: 8 Jul 2015
 *      Author: tony
 */

#include <iostream>
#include <sstream>

#include "Proposition.h"
#include "TIL.h"

namespace PDDL {

std::string TIL::getName() const {
	std::ostringstream output;
	output << "at-" << timestamp;
	std::list<Proposition>::const_iterator addEffItr = addEffects.begin();
	const std::list<Proposition>::const_iterator addEffItrEnd =
			addEffects.end();
	for (; addEffItr != addEffItrEnd; addEffItr++) {
		output << "-" << Proposition::getDecoratedName(*addEffItr);
	}
	if (getDelEffects().size()) {
		output << "-not";
		std::list<Proposition>::const_iterator delEffItr = delEffects.begin();
		const std::list<Proposition>::const_iterator delEffItrEnd =
				delEffects.end();
		for (; delEffItr != delEffItrEnd; delEffItr++) {
			output << "-" << Proposition::getDecoratedName(*delEffItr);
		}
	}
	return output.str();
}

/*
 * TODO: Handle multiple facts, currently there is no "and" operator for multiple facts
 * TODO: Handle both add and remove in the same TIL
 */
std::ostream & operator<<(std::ostream & output, const TIL & til) {
	output << "(at " << til.timestamp << " ";
	//add Add Effect is exist
	if (til.addEffects.size()) {
		std::list<Proposition>::const_iterator addEffItr =
				til.addEffects.begin();
		const std::list<Proposition>::const_iterator addEffItrEnd =
				til.addEffects.end();
		for (; addEffItr != addEffItrEnd; addEffItr++) {
			output << (*addEffItr) << " ";
		}
	} else if (til.delEffects.size()) { //This explicitly means each TIL can either add _OR_ remove facts
		output << "(not ";
		std::list<Proposition>::const_iterator delEffItr =
				til.delEffects.begin();
		const std::list<Proposition>::const_iterator delEffItrEnd =
				til.delEffects.end();
		for (; delEffItr != delEffItrEnd; delEffItr++) {
			output << (*delEffItr) << " ";
		}
		output << ")";
	}
	output << ")";
	return output;
}

bool TIL::TILTimestampComparator(const TIL & first, const TIL & second) {
	if (first.timestamp <= second.timestamp) {
		return true;
	}
	return false;
}

bool TIL::operator==(const TIL & other) {
	//Check if the adds are the same
	if (this->addEffects.size() != other.addEffects.size()) {
		return false;
	}
	std::list<Proposition>::const_iterator thisAddItr =
			this->addEffects.begin();
	std::list<Proposition>::const_iterator othrAddItr =
			other.addEffects.begin();
	for (; thisAddItr != this->addEffects.end(); thisAddItr++, othrAddItr++) {
		Proposition thisAdd = *thisAddItr;
		Proposition othrAdd = *othrAddItr;
		if (thisAdd != othrAdd) {
			return false;
		}
	}
	//Check if the dels are the same
	if (this->delEffects.size() != other.delEffects.size()) {
		return false;
	}
	std::list<Proposition>::const_iterator thisDelItr =
			this->delEffects.begin();
	std::list<Proposition>::const_iterator othrDelItr =
			other.delEffects.begin();
	for (; thisDelItr != this->delEffects.end(); thisDelItr++, othrDelItr++) {
		PDDL::Proposition thisDel = *thisDelItr;
		PDDL::Proposition othrDel = *othrDelItr;
		if (thisDel != othrDel) {
			return false;
		}
	}
	//Check if the timestamps is the same
	if (this->timestamp != other.timestamp) {
		return false;
	}
	//They are the same
	return true;
}

bool TIL::operator!=(const TIL & other) {
	return !((*this) == other);
}

}

