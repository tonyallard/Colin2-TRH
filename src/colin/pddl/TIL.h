/*
 * TIL.h
 *
 *  Created on: 2 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PDDL_TIL_H_
#define COLIN_PDDL_TIL_H_

#include <list>

#include "Literal.h"

namespace PDDL {

class TIL {
private:
	std::list<Literal> addEffects;
	std::list<Literal> delEffects;
	double timestamp;

public:
	TIL(std::list<Literal> addEffects, std::list<Literal> delEffects,
			double timestamp) :
			addEffects(addEffects), delEffects(delEffects), timestamp(timestamp) {
	}
	;
	friend std::ostream & operator<<(std::ostream & output, const TIL & til);
};

}

#endif /* COLIN_PDDL_TIL_H_ */
