/*
 * Literal.cpp
 *
 *  Created on: 8 Jul 2015
 *      Author: tony
 */

#include <iostream>
#include <sstream>

#include "Literal.h"

namespace PDDL {

std::ostream & operator<<(std::ostream & output, const Proposition & literal) {
	output << "(" << literal.name << " ";
	std::list<std::string>::const_iterator argItr = literal.arguments.begin();
	const std::list<std::string>::const_iterator argItrEnd =
			literal.arguments.end();
	for (; argItr != argItrEnd; argItr++) {
		output << (*argItr) << " ";
	}
	output << ")";
	return output;
}

std::string Proposition::getDecoratedName(const Proposition & literal) {
	std::ostringstream output;
	output << literal.name;
	std::list<std::string>::const_iterator argItr = literal.arguments.begin();
	const std::list<std::string>::const_iterator argItrEnd =
			literal.arguments.end();
	for (; argItr != argItrEnd; argItr++) {
		output << "-" << (*argItr);
	}
	return output.str();
}

}

