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
	std::list<Proposition> addEffects;
	std::list<Proposition> delEffects;
	double timestamp;

public:
	TIL(std::list<Proposition> addEffects, std::list<Proposition> delEffects,
			double timestamp) :
			addEffects(addEffects), delEffects(delEffects), timestamp(timestamp) {
	}
	;
	std::string getName() const;
	double getTimestamp() const { return timestamp; };
	const std::list<Proposition> & getAddEffects() const { return addEffects; };
	const std::list<Proposition> & getDelEffects() const { return delEffects; };
	friend std::ostream & operator<<(std::ostream & output, const TIL & til);
	static bool TILTimestampComparator(const TIL & first, const TIL & second);
	friend bool TILTimestampComparator(const TIL & first, const TIL & second);
};

}

#endif /* COLIN_PDDL_TIL_H_ */
