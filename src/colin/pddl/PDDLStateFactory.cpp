/*
 * PDDLStateFactory.cpp
 *
 *  Created on: 2 Jul 2015
 *      Author: tony
 */

#include <string>
#include <list>
#include <sstream>

#include "PDDLStateFactory.h"
#include "PDDLUtils.h"
#include "../FakeTILAction.h"

using namespace Planner;

namespace PDDL {

PDDLState PDDLStateFactory::getPDDLState(const MinimalState & state, std::list<Planner::FFEvent>& plan,
		double timestamp, double heuristic) {
	std::list<Proposition> literals = PDDLStateFactory::getPropositions(state);
	std::list<PNE> pnes = getPNEs(state);
	std::list<TIL> tils = getTILs(state, timestamp);
	std::list<PendingAction> pendingActions = getPendingActions(state, timestamp);
	std::list<string> planPrefix = getPlanPrefix(plan);
	return PDDLState(literals, pnes, tils, pendingActions, planPrefix, heuristic, timestamp);
}

std::list<PDDL::Proposition> PDDLStateFactory::getPropositions(
		const Planner::MinimalState & state) {
	std::list<PDDL::Proposition> literals;
	//Cycle through State Literal Facts
	const StateFacts & stateFacts = state.first;
	StateFacts::const_iterator cfItr = stateFacts.begin();
	const StateFacts::const_iterator cfEnd = stateFacts.end();
	for (; cfItr != cfEnd; cfItr++) {
		int literalID = *cfItr;
		Inst::Literal * aliteral = RPGBuilder::getLiteral(literalID);
		PDDL::Proposition literal = PDDL::getProposition(aliteral);
		literals.push_back(literal);
	}

	//Look for static literals
	std::vector<pair<bool, bool> > staticLiterals =
			Planner::RPGBuilder::getStaticLiterals();
	std::vector<pair<bool, bool> >::const_iterator slItr =
			staticLiterals.begin();
	const std::vector<pair<bool, bool> >::const_iterator slItrEnd =
			staticLiterals.end();
	int i = 0;
	for (; slItr != slItrEnd; slItr++) {
		Inst::Literal * aliteral = RPGBuilder::getLiteral(i++);
		if (slItr->first && slItr->second) {
			PDDL::Proposition literal = PDDL::getProposition(aliteral);
			literals.push_back(literal);
		}
	}
	return literals;
}

std::list<PDDL::PNE> PDDLStateFactory::getPNEs(
		const Planner::MinimalState & state) {
	std::list<PDDL::PNE> pnes;
	//Cycle through PNEs
	const int pneCount = state.secondMin.size();
	for (int i = 0; i < pneCount; i++) {
		Inst::PNE* aPNE = Planner::RPGBuilder::getPNE(i);
		PDDL::PNE pne = PDDL::getPNE(aPNE, state.secondMin[i]);
		pnes.push_back(pne);
	}

	//Manually Add static PNEs
	pnes.push_back(PNE("travel-time", { "v1", "l2", "l1" }, 2.0));
	pnes.push_back(PNE("travel-time", { "v1", "l1", "l2" }, 2.0));
	pnes.push_back(PNE("travel-time", { "v2", "l2", "l3" }, 2.0));
	pnes.push_back(PNE("travel-time", { "v2", "l3", "l2" }, 2.0));
	pnes.push_back(PNE("size", { "C1" }, 1.0));
	pnes.push_back(PNE("load-time", { "v1", "l1" }, 1.0));
	pnes.push_back(PNE("load-time", { "v1", "l2" }, 1.0));
	pnes.push_back(PNE("unload-time", { "v1", "l1" }, 1.0));
	pnes.push_back(PNE("unload-time", { "v1", "l2" }, 1.0));
	pnes.push_back(PNE("load-time", { "v2", "l3" }, 1.0));
	pnes.push_back(PNE("load-time", { "v2", "l2" }, 1.0));
	pnes.push_back(PNE("unload-time", { "v2", "l3" }, 1.0));
	pnes.push_back(PNE("unload-time", { "v2", "l2" }, 1.0));
	pnes.push_back(PNE("cost", { "v1" }, 1.0));
	pnes.push_back(PNE("cost", { "v2" }, 1.0));

	return pnes;
}

std::list<PDDL::TIL> PDDLStateFactory::getTILs(
		const Planner::MinimalState & state, double timestamp) {
	std::list<PDDL::TIL> tils;

	//Cycle thourgh TILs
	list<FakeTILAction> theTILs = Planner::RPGBuilder::getTILs();
	std::list<FakeTILAction>::const_iterator tilItr = theTILs.begin();
	const std::list<FakeTILAction>::const_iterator tilItrEnd = theTILs.end();
	for (; tilItr != tilItrEnd; tilItr++) {
		//Make sure the TIL is still current
		if ((*tilItr).duration < timestamp) {
			continue;
		}
		PDDL::TIL til = PDDL::getTIL(*tilItr, timestamp);
		tils.push_back(til);
	}
	return tils;
}

std::list<PendingProposition> PDDLStateFactory::getPendingPropositions(
		const Planner::MinimalState & state, double timestamp) {
	std::list<PendingProposition> pendingLiterals;
	//Cycle through Facts held up by executing actions (these are effects coming into play)
	std::map<int, std::set<int> >::const_iterator saItr =
			state.startedActions.begin();
	const std::map<int, std::set<int> >::const_iterator saItrEnd =
			state.startedActions.end();
	for (; saItr != saItrEnd; saItr++) {
		//For each action get its conditions
		/*Postconditions*/
		std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions;

		std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > tempConditions =
				getConditions(
						RPGBuilder::getEndPropositionalPreconditions()[saItr->first],
						VAL::time_spec::E_AT_END, true);
		conditions.insert(conditions.end(), tempConditions.begin(),
				tempConditions.end());

		tempConditions =
				getConditions(
						RPGBuilder::getEndNegativePropositionalPreconditions()[saItr->first],
						VAL::time_spec::E_AT_END, false);
		conditions.insert(conditions.end(), tempConditions.begin(),
				tempConditions.end());

		/*Invariant Conditions*/
		tempConditions =
				getConditions(
						RPGBuilder::getInvariantPropositionalPreconditions()[saItr->first],
						VAL::time_spec::E_OVER_ALL, true);
		conditions.insert(conditions.end(), tempConditions.begin(),
				tempConditions.end());

		tempConditions =
				getConditions(
						RPGBuilder::getInvariantNegativePropositionalPreconditions()[saItr->first],
						VAL::time_spec::E_OVER_ALL, false);
		conditions.insert(conditions.end(), tempConditions.begin(),
				tempConditions.end());

		// Literals Added by Action
		std::list<Inst::Literal*> adds =
				RPGBuilder::getEndPropositionAdds()[saItr->first];
		std::list<Inst::Literal*>::const_iterator addItr = adds.begin();
		const std::list<Inst::Literal*>::const_iterator addItrEnd = adds.end();
		for (; addItr != addItrEnd; addItr++) {
			PendingProposition pendingLiteral = PDDL::getPendingProposition(
					*addItr, conditions, (*saItr->second.begin() - timestamp),
					true);
			pendingLiterals.push_back(pendingLiteral);
		}

		// Literals Deleted by Action
		std::list<Inst::Literal*> deletes =
				RPGBuilder::getEndPropositionDeletes()[saItr->first];
		std::list<Inst::Literal*>::const_iterator delItr = deletes.begin();
		const std::list<Inst::Literal*>::const_iterator delItrEnd =
				deletes.end();
		for (; delItr != delItrEnd; delItr++) {
			PendingProposition pendingLiteral = PDDL::getPendingProposition(
					*delItr, conditions, (*saItr->second.begin() - timestamp),
					false);
			pendingLiterals.push_back(pendingLiteral);
		}
	}
	return pendingLiterals;
}

std::list<PDDL::PendingPNE> PDDLStateFactory::getPendingPNEs(
		const Planner::MinimalState & state, double timestamp) {
	std::list<PendingPNE> pendingPNEs;
	//Cycle through Facts held up by executing actions (these are effects coming into play)
	std::map<int, std::set<int> >::const_iterator saItr =
			state.startedActions.begin();
	const std::map<int, std::set<int> >::const_iterator saItrEnd =
			state.startedActions.end();
	for (; saItr != saItrEnd; saItr++) {
		//For each action get its conditions
		/*Postconditions*/
		std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions;

		std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > tempConditions =
				getConditions(
						RPGBuilder::getEndPropositionalPreconditions()[saItr->first],
						VAL::time_spec::E_AT_END, true);
		conditions.insert(conditions.end(), tempConditions.begin(),
				tempConditions.end());

		tempConditions =
				getConditions(
						RPGBuilder::getEndNegativePropositionalPreconditions()[saItr->first],
						VAL::time_spec::E_AT_END, false);
		conditions.insert(conditions.end(), tempConditions.begin(),
				tempConditions.end());

		/*Invariant Conditions*/
		tempConditions =
				getConditions(
						RPGBuilder::getInvariantPropositionalPreconditions()[saItr->first],
						VAL::time_spec::E_OVER_ALL, true);
		conditions.insert(conditions.end(), tempConditions.begin(),
				tempConditions.end());

		tempConditions =
				getConditions(
						RPGBuilder::getInvariantNegativePropositionalPreconditions()[saItr->first],
						VAL::time_spec::E_OVER_ALL, false);
		conditions.insert(conditions.end(), tempConditions.begin(),
				tempConditions.end());

		// Get action duration
		double minDur = RPGBuilder::getOpMinDuration(saItr->first, 1);
		double maxDur = RPGBuilder::getOpMaxDuration(saItr->first, 1);

		// PNEs Added by Action
		std::list<int> numerics = RPGBuilder::getEndEffNumerics()[saItr->first];
		std::list<int>::const_iterator numItr = numerics.begin();
		const std::list<int>::const_iterator numItrEnd = numerics.end();
		for (; numItr != numItrEnd; numItr++) {
			PendingPNE pendingPNE = PDDLStateFactory::getPendingPNE(state,
					*numItr, conditions, minDur, maxDur);
			pendingPNEs.push_back(pendingPNE);
			cout << pendingPNE.toActionString() << "\n";
		}
	}
	return pendingPNEs;
}

PDDL::PendingPNE PDDLStateFactory::getPendingPNE(
		const Planner::MinimalState & state, int numeric,
		std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions,
		double minDur, double maxDur) {
	//Get the numeric effect information
	RPGBuilder::RPGNumericEffect & effect = RPGBuilder::getNumericEff()[numeric];
	//Apply the numeric effect to get new fluent values
	pair<double, double> result = effect.applyEffectMinMax(state.secondMin,
			state.secondMax, minDur, maxDur);
	//Determine the PNE that the values belong
	Inst::PNE* aPNE = Planner::RPGBuilder::getPNE(effect.fluentIndex);
	PDDL::PNE pne = PDDL::getPNE(aPNE, result.first);
	PendingPNE pendingPNE(pne.getName(), pne.getArguments(), pne.getValue(),
			conditions, minDur);
	return pendingPNE;
}

std::list<PDDL::PendingAction> PDDLStateFactory::getPendingActions(
		const Planner::MinimalState & state, double timestamp) {
	std::list<PendingAction> pendingActions;
	//Cycle through Facts held up by executing actions (these are effects coming into play)
	std::map<int, std::set<int> >::const_iterator saItr =
			state.startedActions.begin();
	const std::map<int, std::set<int> >::const_iterator saItrEnd =
			state.startedActions.end();
	for (; saItr != saItrEnd; saItr++) {

		//For each action get its conditions
		/*Postconditions*/
		std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions;

		std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > tempConditions =
				getConditions(
						RPGBuilder::getEndPropositionalPreconditions()[saItr->first],
						VAL::time_spec::E_AT_END, true);
		conditions.insert(conditions.end(), tempConditions.begin(),
				tempConditions.end());

		tempConditions =
				getConditions(
						RPGBuilder::getEndNegativePropositionalPreconditions()[saItr->first],
						VAL::time_spec::E_AT_END, false);
		conditions.insert(conditions.end(), tempConditions.begin(),
				tempConditions.end());

		/*Invariant Conditions*/
		tempConditions =
				getConditions(
						RPGBuilder::getInvariantPropositionalPreconditions()[saItr->first],
						VAL::time_spec::E_OVER_ALL, true);
		conditions.insert(conditions.end(), tempConditions.begin(),
				tempConditions.end());

		tempConditions =
				getConditions(
						RPGBuilder::getInvariantNegativePropositionalPreconditions()[saItr->first],
						VAL::time_spec::E_OVER_ALL, false);
		conditions.insert(conditions.end(), tempConditions.begin(),
				tempConditions.end());

		// Get action duration
		double minDur = RPGBuilder::getOpMinDuration(saItr->first, 1);
		double maxDur = RPGBuilder::getOpMaxDuration(saItr->first, 1);

		// Literals Added by Action
		std::list<PDDL::Proposition> propositionalAddEffects;
		std::list<Inst::Literal*> adds =
				RPGBuilder::getEndPropositionAdds()[saItr->first];
		std::list<Inst::Literal*>::const_iterator addItr = adds.begin();
		const std::list<Inst::Literal*>::const_iterator addItrEnd = adds.end();
		for (; addItr != addItrEnd; addItr++) {
			Proposition prop = PDDL::getProposition(*addItr);
			propositionalAddEffects.push_back(prop);
		}

		// Literals Deleted by Action
		std::list<PDDL::Proposition> propositionalDelEffects;
		std::list<Inst::Literal*> deletes =
				RPGBuilder::getEndPropositionDeletes()[saItr->first];
		std::list<Inst::Literal*>::const_iterator delItr = deletes.begin();
		const std::list<Inst::Literal*>::const_iterator delItrEnd =
				deletes.end();
		for (; delItr != delItrEnd; delItr++) {
			Proposition prop = PDDL::getProposition(*delItr);
			propositionalDelEffects.push_back(prop);
		}

		// PNEs Added by Action
		std::list<PDDL::PNE> pneEffects;
		std::list<int> numerics = RPGBuilder::getEndEffNumerics()[saItr->first];
		std::list<int>::const_iterator numItr = numerics.begin();
		const std::list<int>::const_iterator numItrEnd = numerics.end();
		for (; numItr != numItrEnd; numItr++) {
			//Get the numeric effect information
			RPGBuilder::RPGNumericEffect & effect =
					RPGBuilder::getNumericEff()[*numItr];
			//Apply the numeric effect to get new fluent values
			pair<double, double> result = effect.applyEffectMinMax(
					state.secondMin, state.secondMax, minDur, maxDur);
			//Determine the PNE that the values belong
			Inst::PNE* aPNE = Planner::RPGBuilder::getPNE(effect.fluentIndex);
			PDDL::PNE pne = PDDL::getPNE(aPNE, result.first);
			pneEffects.push_back(pne);
		}
		std::string name = PDDL::getActionName(saItr->first);
		PendingAction pendingAction (name, propositionalAddEffects, propositionalDelEffects, pneEffects, conditions, minDur);
		pendingActions.push_back(pendingAction);
	}
	return pendingActions;
}

std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > PDDLStateFactory::getConditions(
		std::list<Inst::Literal*> conditionLiterals,
		VAL::time_spec timeQualifier, bool isPositive) {
	std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions;

	std::list<Inst::Literal*>::const_iterator condItr =
			conditionLiterals.begin();
	std::list<Inst::Literal*>::const_iterator condItrEnd =
			conditionLiterals.end();

	for (; condItr != condItrEnd; condItr++) {
		PDDL::Proposition literal = PDDL::getProposition(*condItr);
		conditions.push_back(
				pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> >(literal,
						std::pair<VAL::time_spec, bool>(timeQualifier,
								isPositive)));
	}
	return conditions;
}

}
