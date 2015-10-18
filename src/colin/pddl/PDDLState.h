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

#include "Proposition.h"
#include "PNE.h"
#include "TIL.h"
#include "PendingAction.h"

namespace PDDL {

class PDDLState {

private:
	std::list<Proposition> literals;
	std::list<PNE> pnes;
	std::list<TIL> tils;
	std::list<PendingAction> pendingActions;
	std::list<string> planPrefix;
	double timestamp;
	double heuristic;

	string getPlanPrefixString();

public:
	PDDLState(std::list<Proposition> literals, std::list<PNE> pnes,
			std::list<TIL> tils, std::list<PendingAction> pendingActions,
			std::list<string> planPrefix, double heuristic, double timestamp) :
			literals(literals), pnes(pnes), tils(tils), pendingActions(
					pendingActions), planPrefix(planPrefix), heuristic(
					heuristic), timestamp(timestamp) {
	}
	;
	std::string toString();
	std::string getLiteralString();
	std::string getPNEString();
	std::string getTILLiteralString();
	std::string getDomainString();
	std::string getDeTiledDomainString();
	inline double getTimestamp() {
		return timestamp;
	}
	void writeStateToFile(std::string filePath, std::string fileName);
	void writeDeTILedStateToFile(std::string filePath, std::string fileName);
	void writeDeTILedDomainToFile(string filePath, string fileName);

};

}

#endif /* COLIN_PDDLSTATE_H_ */
