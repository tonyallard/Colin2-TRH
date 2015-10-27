/*
 * PDDLStateFactory.cpp
 *
 *  Created on: 2 Jul 2015
 *      Author: tony
 */

#include <string>
#include <list>
#include <set>
#include <sstream>

#include "PDDLStateFactory.h"
#include "PDDLUtils.h"
#include "../FakeTILAction.h"

using namespace Planner;

namespace PDDL {

PDDLState PDDLStateFactory::getPDDLState(const MinimalState & state,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic) {
	std::list<Proposition> propositions = PDDLStateFactory::getPropositions(
			state);
	std::list<PNE> pnes = getPNEs(state);
	std::list<TIL> tils = getTILs(state, timestamp);
	std::list<PendingAction> pendingActions = getPendingActions(state,
			timestamp);
	addExtraPropositionsForPendingActions(pendingActions, propositions);
	std::list<string> planPrefix = getPlanPrefix(plan);
	return PDDLState(propositions, pnes, tils, pendingActions, planPrefix,
			heuristic, timestamp);
}

/**
 * Cycles through each pending action, gets the parameters,
 * dumps it all into one set for uniqueness and then creates the
 * required propositions
 */
void PDDLStateFactory::addExtraPropositionsForPendingActions(
		const std::list<PendingAction> & pendingActions,
		std::list<Proposition> & propositions) {
	std::set<PDDLObject> parameters;
	std::list<PendingAction>::const_iterator pActItr = pendingActions.begin();
	for (; pActItr != pendingActions.end(); pActItr++) {
		parameters.insert(pActItr->getParameters().begin(),
				pActItr->getParameters().end());
	}
	std::list<Proposition> extraProps = getRequiredPropositions(parameters);
	propositions.insert(propositions.end(), extraProps.begin(),
			extraProps.end());
}

/**
 * Creates all the required propositions for the pending actions
 * This ensures the correct objects get used for the actions
 */
std::list<Proposition> PDDLStateFactory::getRequiredPropositions(
		std::set<PDDLObject> & parameters) {
	std::list<Proposition> requiredProps;
	std::set<PDDLObject>::const_iterator paramItr = parameters.begin();
	for (; paramItr != parameters.end(); paramItr++) {
		list<string> args;
		args.push_back(paramItr->getName());
		Proposition prop(MMCRDomainFactory::REQUIRED_PROPOSITION, args);
		requiredProps.push_back(prop);
	}
	return requiredProps;
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

/**
 * Get the actions in a state that are in the middle of executing.
 * For example the start snap action has been executed, but not the end snap action
 */
std::list<PDDL::PendingAction> PDDLStateFactory::getPendingActions(
		const Planner::MinimalState & state, double timestamp) {
	std::list<PendingAction> pendingActions;
	//Cycle through Facts held up by executing actions (these are effects coming into play)
	std::map<int, std::set<int> >::const_iterator saItr =
			state.startedActions.begin();
	const std::map<int, std::set<int> >::const_iterator saItrEnd =
			state.startedActions.end();
	for (; saItr != saItrEnd; saItr++) {

		std::set<PDDLObject> parameters;

		//For each action get its conditions
		std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions =
				getConditions(saItr->first, parameters);

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
			parameters = extractParameters(*addItr, parameters);
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
			parameters = extractParameters(*delItr, parameters);
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
			//Ensure the parameteres of the PNE are captured
			parameters = extractParameters(aPNE, parameters);
		}
		std::string name = PDDL::getActionName(saItr->first);
		PendingAction pendingAction(name, parameters, propositionalAddEffects,
				propositionalDelEffects, pneEffects, conditions, minDur);
		pendingActions.push_back(pendingAction);
	}
	return pendingActions;
}

/**
 * Get all conditions and return parameters
 */
std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > PDDLStateFactory::getConditions(
		int actionID, std::set<PDDLObject> & parameters) {

	std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions;

	/*Postconditions*/
	std::list<Inst::Literal*> positivePostConditions =
			RPGBuilder::getEndPropositionalPreconditions()[actionID];
	std::list<Inst::Literal*> negativePostConditions =
			RPGBuilder::getEndNegativePropositionalPreconditions()[actionID];

	std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > tempConditions =
			convertLiterals_AddSignAndTime(positivePostConditions,
					VAL::time_spec::E_AT_END, true, parameters);
	conditions.insert(conditions.end(), tempConditions.begin(),
			tempConditions.end());

	tempConditions = convertLiterals_AddSignAndTime(negativePostConditions,
			VAL::time_spec::E_AT_END, false, parameters);
	conditions.insert(conditions.end(), tempConditions.begin(),
			tempConditions.end());

	/*Invariant Conditions*/
	std::list<Inst::Literal*> positiveInvariantConditions =
			RPGBuilder::getInvariantPropositionalPreconditions()[actionID];
	std::list<Inst::Literal*> negativeInvariantConditions =
			RPGBuilder::getInvariantNegativePropositionalPreconditions()[actionID];
	tempConditions = convertLiterals_AddSignAndTime(positiveInvariantConditions,
			VAL::time_spec::E_OVER_ALL, true, parameters);
	conditions.insert(conditions.end(), tempConditions.begin(),
			tempConditions.end());

	tempConditions = convertLiterals_AddSignAndTime(negativeInvariantConditions,
			VAL::time_spec::E_OVER_ALL, false, parameters);
	conditions.insert(conditions.end(), tempConditions.begin(),
			tempConditions.end());
	return conditions;
}

/**
 * Convert Coling Literals to our datamodel and add their sign and when they apply, with respect to the action
 */
std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > PDDLStateFactory::convertLiterals_AddSignAndTime(
		std::list<Inst::Literal*> literals, VAL::time_spec timeQualifier,
		bool isPositive, std::set<PDDLObject> & parameters) {
	std::list<pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions;

	std::list<Inst::Literal*>::const_iterator condItr = literals.begin();
	std::list<Inst::Literal*>::const_iterator condItrEnd = literals.end();

	for (; condItr != condItrEnd; condItr++) {
		std::set<PDDLObject> params = PDDL::extractParameters(*condItr,
				parameters);
		PDDL::Proposition literal = PDDL::getProposition(*condItr);
		conditions.push_back(
				pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> >(
						literal,
						std::pair<VAL::time_spec, bool>(timeQualifier,
								isPositive)));
	}
	return conditions;
}

}
