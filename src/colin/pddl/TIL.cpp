/*
 * TIL.cpp
 *
 *  Created on: 8 Jul 2015
 *      Author: tony
 */

#include <iostream>
#include "TIL.h"

namespace PDDL {

/*
 * TODO: Handle multiple facts
 * TODO: Handle both add and remove in the same TIL
 */
std::ostream & operator<<(std::ostream & output, const TIL & til) {
	output << "(at " << til.timestamp << " ";
	//add Add Effect is exist
	if (til.addEffects.size()) {
		std::list<Literal>::const_iterator addEffItr = til.addEffects.begin();
		const std::list<Literal>::const_iterator addEffItrEnd =
				til.addEffects.end();
		for (; addEffItr != addEffItrEnd; addEffItr++) {
			output << (*addEffItr) << " ";
		}
	} else if (til.delEffects.size()) { //This explicitly means each TIL can either add _OR_ remove facts
		output << "(not ";
		std::list<Literal>::const_iterator delEffItr = til.delEffects.begin();
		const std::list<Literal>::const_iterator delEffItrEnd =
				til.delEffects.end();
		for (; delEffItr != delEffItrEnd; delEffItr++) {
			output << (*delEffItr) << " ";
		}
		output << ")";
	}
	output << ")";
	return output;
}

}

