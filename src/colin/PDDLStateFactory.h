/*
 * PDDLStateFactory.h
 *
 *  Created on: 2 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PDDLSTATEFACTORY_H_
#define COLIN_PDDLSTATEFACTORY_H_

#include <list>

#include "Literal.h"
#include "PendingProposition.h"
#include "PendingPNE.h"
#include "PDDLState.h"
#include "minimalstate.h"
#include "RPGBuilder.h"
#include "FFEvent.h"


namespace PDDL {

class PDDLStateFactory {
public:
	static PDDLState getPDDLState(const Planner::MinimalState & state, std::list<Planner::FFEvent>& plan, double timestamp, double heuristic);

private:
	static std::list<PDDL::Proposition> getLiterals(const Planner::MinimalState & state);
	static PDDL::Proposition getLiteral(const Inst::Literal * aLiteral);

	static std::list<PDDL::PNE> getPNEs(const Planner::MinimalState & state);
	static PDDL::PNE getPNE(const Inst::PNE * aPNE, double value);

	static std::list<PDDL::TIL> getTILs(const Planner::MinimalState & state, double timestamp);
	static PDDL::TIL getTIL(Planner::FakeTILAction aTIL, double aTimestamp);

	static std::list<PDDL::PendingProposition> getPendingLiterals(const Planner::MinimalState & state, double timestamp);
	static PDDL::PendingProposition getPendingLiteral(const Inst::Literal * aLiteral, std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions, double timestamp, bool isPositive);

	static std::list<PDDL::PendingPNE> getPendingPNEs(const Planner::MinimalState & state, double timestamp);
	static PDDL::PendingPNE getPendingPNE(const Planner::MinimalState & state, int numeric, std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions, double minDur, double maxDur);

	static std::list<PDDL::PendingAction> getPendingActions(const Planner::MinimalState & state, double timestamp);

	static std::list<std::string> getPlanPrefix(std::list<Planner::FFEvent>& plan);

	static std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > getConditions(std::list<Inst::Literal*> conditionLiterals, VAL::time_spec timeQualifier, bool isPositive);
	static std::string getActionName(int actionNum);
};

}

#endif /* COLIN_PDDLSTATEFACTORY_H_ */
