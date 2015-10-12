/*
 * PNE.cpp
 *
 *  Created on: 8 Jul 2015
 *      Author: tony
 */

#include <iostream>
#include <sstream>

#include "PNE.h"

using namespace std;
namespace PDDL {

std::string PNE::toActionEffectString() const {
	ostringstream output;
	output << "(assign (" << name;
	std::list<std::string>::const_iterator argItr = arguments.begin();
	const std::list<std::string>::const_iterator argItrEnd = arguments.end();
	for (; argItr != argItrEnd; argItr++) {
		output << " " << (*argItr);
	}
	output << ") " << value << ")";
	return output.str();
}

bool PNE::operator==(const PNE & other) {
	//Check the name is the same
	if (this->name != other.name) {
		 return false;
	}
	//Check the arguments are the same
	if (this->arguments.size() != other.arguments.size()) {
		return false;
	}
	std::list<std::string>::const_iterator thisArgItr = this->arguments.begin();
	std::list<std::string>::const_iterator othrArgItr = other.arguments.begin();
	for (; thisArgItr != this->arguments.end(); thisArgItr++, othrArgItr++) {
		std::string thisArg = *thisArgItr;
		std::string othrArg = *othrArgItr;
		if (thisArg != othrArg) {
			return false;
		}
	}
	//Check the value is the same
	if (this->value != other.value) {
		return false;
	}
	return true;
}

bool PNE::operator!=(const PNE & other) {
	return !((*this) == other);
}

std::ostream & operator<<(std::ostream & output, const PNE & pne) {
	output << "(= (" << pne.name << " ";
	std::list<std::string>::const_iterator argItr = pne.arguments.begin();
	const std::list<std::string>::const_iterator argItrEnd =
			pne.arguments.end();
	for (; argItr != argItrEnd; argItr++) {
		output << (*argItr) << " ";
	}
	output << ") " << pne.value << ")";
	return output;
}

}

