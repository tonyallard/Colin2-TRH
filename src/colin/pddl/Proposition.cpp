/*
 * Proposition.cpp
 *
 *  Created on: 8 Jul 2015
 *      Author: tony
 */

#include <iostream>
#include <sstream>

#include "Proposition.h"

namespace PDDL {

std::ostream & operator<<(std::ostream & output, const Proposition & proposition) {
	output << "(" << proposition.name << " ";
	std::list<std::string>::const_iterator argItr = proposition.arguments.begin();
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
	std::list<std::string>::const_iterator argItr = proposition.arguments.begin();
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
	std::list<std::string>::const_iterator otherArgItr = other.arguments.begin();
	for (; argItr != argItrEnd; argItr++, otherArgItr++) {
		if (argItr->compare(*otherArgItr)) {
			return false;
		}
	}
	return true;
}

bool Proposition::operator!=(const Proposition & other) {
	return !((*this) == other);
}

}

