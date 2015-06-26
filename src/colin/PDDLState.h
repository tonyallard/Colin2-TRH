/*
 * PDDLState.h
 *
 *  Created on: 19 Jun 2015
 *      Author: tony
 */

#ifndef COLIN_PDDLSTATE_H_
#define COLIN_PDDLSTATE_H_

#include <string>

#include "ExtendedMinimalState.h"

namespace Planner {

class PDDLState {

private:
	string literals;
	string fluents;
	string tils;
	string metric;
	double timeStamp;
	double heuristic;

public:
	PDDLState (const MinimalState & state, double timeStamp, double heuristic);
	std::string toString();
	inline double getTimeStamp() { return timeStamp; }
	void writeToFile(string fileName);

};

}

#endif /* COLIN_PDDLSTATE_H_ */
