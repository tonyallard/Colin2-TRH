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

#include "../minimalstate.h"
#include "../RPGBuilder.h"
#include "../FFEvent.h"

namespace PDDL {

class PDDLStateFactory {
public:

	PDDLStateFactory(const Planner::MinimalState & initialState, std::list<std::pair<std::string, std::string> > constants);

	PDDLState getPDDLState(const Planner::MinimalState & state,
			std::list<Planner::FFEvent>& plan, double timestamp,
			double heuristic);
	std::list<PDDL::Proposition> getPropositions(
					const Planner::MinimalState & state, std::set<PDDLObject> & objectSymbolTable);
private:
	std::list<std::pair<std::string, std::string> > constants;
	std::list<PDDL::Proposition> staticPropositions;
	std::list<PDDL::PNE> staticPNEs;
	std::set<PDDLObject> objectParameterTable;
	std::list<PDDL::Proposition> goals;
	PDDL::Metric metric;


	std::list<PDDL::Proposition> getStaticPropositions(
			std::list<PDDL::Proposition> & dynamicLiterals, std::set<PDDLObject> & objectSymbolTable);
	std::list<PDDL::PNE> getPNEs(const Planner::MinimalState & state, std::set<PDDLObject> & objectSymbolTable);
	std::list<PDDL::PNE> getStaticPNEs(
			std::list<PDDL::PNE> dynamicPNEs);
	std::list<PDDL::Proposition> getPropositionalGoals();
	PDDL::Metric getMetric();

	std::list<PDDL::TIL> getTILs(const Planner::MinimalState & state,
			double timestamp, std::set<PDDLObject> & objectSymbolTable);
	std::list<PDDL::PendingAction> getPendingActions(
			const Planner::MinimalState & state, double timestamp, std::set<PDDLObject> & objectSymbolTable);

	void addRequiredPropositionsForPendingActions(
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
