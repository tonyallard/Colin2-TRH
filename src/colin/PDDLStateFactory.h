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
#include "PDDLState.h"
#include "minimalstate.h"
#include "RPGBuilder.h"


namespace PDDL {

class PDDLStateFactory {
public:
	static PDDLState getPDDLState(const Planner::MinimalState & state, double timestamp, double heuristic);

private:
	static std::list<PDDL::Literal> getLiterals(const Planner::MinimalState & state);
	static PDDL::Literal getLiteral(const Inst::Literal * aLiteral);

	static std::list<PDDL::PNE> getPNEs(const Planner::MinimalState & state);
	static PDDL::PNE getPNE(const Inst::PNE * aPNE, double value);

	static std::list<PDDL::TIL> getTILs(const Planner::MinimalState & state, double timestamp);
	static PDDL::TIL getTIL(Planner::FakeTILAction aTIL, double aTimestamp);

	static std::list<PDDL::PendingLiteral> getPendingLiterals(const Planner::MinimalState & state, double timestamp);
	static PDDL::PendingLiteral getPendingLiteral(const Inst::Literal * aLiteral, std::list<std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions, double timestamp, bool isPositive);

	static std::list<PDDL::PendingPNE> getPendingPNEs(const Planner::MinimalState & state, double timestamp);
	static PDDL::PendingPNE getPendingPNE(const Planner::MinimalState & state, int numeric, std::list<std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions, double minDur, double maxDur);

	static std::list<pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > getConditions(std::list<Inst::Literal*> conditionLiterals, VAL::time_spec timeQualifier, bool isPositive);
};

}

#endif /* COLIN_PDDLSTATEFACTORY_H_ */
