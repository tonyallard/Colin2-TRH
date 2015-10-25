/*
 * Proposition.cpp
 *
 *  Created on: 8 Jul 2015
 *      Author: tony
 */

#include <iostream>
#include <sstream>
#include <map>
#include <string>

#include "Proposition.h"

namespace PDDL {

/**
 * Output the proposition using parameters according to the parameter table
 */
std::string Proposition::toParameterisedString(
		const std::map<const PDDLObject *, std::string> & parameterTable) const {
	std::ostringstream output;
	output << "(" << name << " ";
	std::list<std::string>::const_iterator argItr = arguments.begin();
	const std::list<std::string>::const_iterator argItrEnd = arguments.end();
	for (; argItr != argItrEnd; argItr++) {
		// Find the corresponding parameters
		std::map<const PDDLObject *, std::string>::const_iterator paramItr = parameterTable.begin();
		for (; paramItr != parameterTable.end(); paramItr++) {
			std::pair<const PDDLObject *, std::string> param = *paramItr;
			if (param.first->getName().compare(*argItr) == 0) {
				output << param.second << " ";
			}
		}
	}
	output << ")";
	return output.str();
}

std::ostream & operator<<(std::ostream & output,
		const Proposition & proposition) {
	output << "(" << proposition.name << " ";
	std::list<std::string>::const_iterator argItr =
			proposition.arguments.begin();
	const std::list<std::string>::const_iterator argItrEnd =
			proposition.arguments.end();
	for (; argItr != argItrEnd; argItr++) {
		output << (*argItr) << " ";
	}
	output << ")";
	return output;
}

std::string Proposition::getDecoratedName(const Proposition & proposition) {
	std::ostringstream output;
	output << proposition.name;
	std::list<std::string>::const_iterator argItr =
			proposition.arguments.begin();
	const std::list<std::string>::const_iterator argItrEnd =
			proposition.arguments.end();
	for (; argItr != argItrEnd; argItr++) {
		output << "-" << (*argItr);
	}
	return output.str();
}

bool Proposition::operator==(const Proposition & other) {
	if (name.compare(other.name)) {
		return false;
	}
	if (arguments.size() != other.arguments.size()) {
		return false;
	}
	std::list<std::string>::const_iterator argItr = arguments.begin();
	const std::list<std::string>::const_iterator argItrEnd = arguments.end();
	std::list<std::string>::const_iterator otherArgItr =
			other.arguments.begin();
	for (; argItr != argItrEnd; argItr++, otherArgItr++) {
		if (argItr->compare(*otherArgItr)) {
			return false;
		}
	}
	return true;
}

}

