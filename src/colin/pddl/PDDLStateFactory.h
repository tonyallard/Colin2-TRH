/*
 * PDDLStateFactory.h
 *
 *  Created on: 2 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PDDLSTATEFACTORY_H_
#define COLIN_PDDLSTATEFACTORY_H_

#include <list>
#include <set>

#include "PDDLObject.h"
#include "Proposition.h"
#include "PendingPNE.h"
#include "PDDLState.h"
#include "PendingProposition.h"
#include "MMCRDomainFactory.h"

#include "../minimalstate.h"
#include "../RPGBuilder.h"
#include "../FFEvent.h"

namespace PDDL {

class PDDLStateFactory {
public:

	PDDLStateFactory(const Planner::MinimalState & initialState);

	PDDLState getPDDLState(const Planner::MinimalState & state,
			std::list<Planner::FFEvent>& plan, double timestamp,
			double heuristic);
private:
	std::list<PDDL::Proposition> staticPropositions;
	std::list<PDDL::PNE> staticPNEs;

	std::list<PDDL::Proposition> getPropositions(
			const Planner::MinimalState & state);
	std::list<PDDL::Proposition> getStaticPropositions(
			std::list<PDDL::Proposition> & dynamicLiterals);
	std::list<PDDL::PNE> getPNEs(const Planner::MinimalState & state);
	std::list<PDDL::PNE> getStaticPNEs(
			std::list<PDDL::PNE> dynamicPNEs);
//	double extract
	std::list<PDDL::TIL> getTILs(const Planner::MinimalState & state,
			double timestamp);
	std::list<PDDL::PendingAction> getPendingActions(
			const Planner::MinimalState & state, double timestamp);

	void addExtraPropositionsForPendingActions(
			const std::list<PendingAction> & pendingActions,
			std::list<Proposition> & propositions);
	std::list<Proposition> getRequiredPropositions(
			const std::set<PDDLObject> & parameters, std::string actionName);

	/*Methods to get condition literals*/
	std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > getConditions(
			int actionID, std::set<PDDLObject> & parameters);

	std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > convertLiterals_AddSignAndTime(
			std::list<Inst::Literal*> literals, VAL::time_spec timeQualifier,
			bool isPositive, std::set<PDDLObject> & parameters);

};

}

#endif /* COLIN_PDDLSTATEFACTORY_H_ */
