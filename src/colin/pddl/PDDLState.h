/*
 * PDDLState.h
 *
 *  Created on: 19 Jun 2015
 *      Author: tony
 */

#ifndef COLIN_PDDLSTATE_H_
#define COLIN_PDDLSTATE_H_

#include <string>
#include <list>

#include "Literal.h"
#include "PNE.h"
#include "TIL.h"
#include "PendingLiteral.h"
#include "PendingPNE.h"
#include "PendingAction.h"

namespace PDDL {

class PDDLState {

private:
	std::list<Literal> literals;
	std::list<PNE> pnes;
	std::list<TIL> tils;
//	string metric;
	double timestamp;
	double heuristic;

public:
	PDDLState (std::list<Literal> literals, std::list<PNE> pnes, std::list<TIL> tils, double heuristic, double timestamp) : literals(literals), pnes(pnes), tils(tils), heuristic(heuristic), timestamp(timestamp) {};
	std::string toString();
	inline double getTimestamp() { return timestamp; }
	void writeToFile(std::string filePath, std::string fileName);

};

}

#endif /* COLIN_PDDLSTATE_H_ */
