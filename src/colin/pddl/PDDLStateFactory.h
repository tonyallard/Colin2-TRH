/*
 * PDDLStateFactory.h
 *
 *  Created on: 2 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PDDLSTATEFACTORY_H_
#define COLIN_PDDLSTATEFACTORY_H_

#include <list>

#include "PendingPNE.h"
#include "PDDLState.h"
#include "PendingProposition.h"
#include "Proposition.h"

#include "../minimalstate.h"
#include "../RPGBuilder.h"
#include "../FFEvent.h"


namespace PDDL {

class PDDLStateFactory {
public:
	static PDDLState getPDDLState(const Planner::MinimalState & state, std::list<Planner::FFEvent>& plan, double timestamp, double heuristic);

private:
	static std::list<PDDL::Proposition> getPropositions(const Planner::MinimalState & state);
	static std::list<PDDL::PNE> getPNEs(const Planner::MinimalState & state);
	static std::list<PDDL::TIL> getTILs(const Planner::MinimalState & state, double timestamp);
	static std::list<PDDL::PendingProposition> getPendingPropositions(const Planner::MinimalState & state, double timestamp);
	static std::list<PDDL::PendingPNE> getPendingPNEs(const Planner::MinimalState & state, double timestamp);
	static PDDL::PendingPNE getPendingPNE(const Planner::MinimalState & state, int numeric, std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions, double minDur, double maxDur);
	static std::list<PDDL::PendingAction> getPendingActions(const Planner::MinimalState & state, double timestamp);

	static std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > getConditions(std::list<Inst::Literal*> conditionLiterals, VAL::time_spec timeQualifier, bool isPositive);
};

}

#endif /* COLIN_PDDLSTATEFACTORY_H_ */
