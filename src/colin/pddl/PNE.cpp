/*
 * PNE.cpp
 *
 *  Created on: 8 Jul 2015
 *      Author: tony
 */

#include <iostream>

#include "PNE.h"

namespace PDDL {

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

