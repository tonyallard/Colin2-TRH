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
#include "PDDLDomainFactory.h"
#include "PropositionFactory.h"

#include "../RPGBuilder.h"

using namespace std;
using namespace Planner;

namespace PDDL {

PDDLStateFactory::PDDLStateFactory(const Planner::MinimalState & initialState,
		std::list<std::pair<std::string, std::string> > constants) {
	this->constants = constants;
	std::list<PDDL::Proposition> stdPropositions = getPropositions(initialState,
			objectParameterTable);
	staticPropositions = getStaticPropositions(stdPropositions,
			objectParameterTable);
	std::list<PDDL::PNE> stdPNEs = getPNEs(initialState, objectParameterTable);
	staticPNEs = getStaticPNEs(stdPNEs);
	goals = getPropositionalGoals();
	metric = getMetric();
}

PDDL::Metric PDDLStateFactory::getMetric() {
	Planner::RPGBuilder::Metric * metric = Planner::RPGBuilder::getMetric();
	list<std::string> variables;
	list<int>::const_iterator varItr = metric->variables.begin();
	for (; varItr != metric->variables.end(); varItr++) {
		//Colin seems to fill PNE index lists with negative values
		//if the PNE is total-time, so define explicitly
		std::string var = "(" + PDDL::DEFAULT_METRIC_PNE + ")";
		if ((*varItr >= 0) && (*varItr < Planner::RPGBuilder::getPNECount())) {
			Inst::PNE* aPNE = Planner::RPGBuilder::getPNE(*varItr);
			var = "(" + aPNE->getHead()->getName() + ")";
		}
		variables.push_back(var);
	}
	PDDL::Metric aMetric(metric->minimise, variables);
	return aMetric;
}

/**
 * Gets the propositional Goals.
 * N.B. We say propositional because we assume they are positive.
 * FIXME Add support for negative propositonal goals
 */
std::list<Proposition> PDDLStateFactory::getPropositionalGoals() {
	std::list<Proposition> goals;
	std::list<Inst::Literal*>::const_iterator goalItr =
			Planner::RPGBuilder::getLiteralGoals().begin();
	const std::list<Inst::Literal*>::const_iterator goalItrEnd =
			Planner::RPGBuilder::getLiteralGoals().end();
	for (; goalItr != goalItrEnd; goalItr++) {
		Proposition prop = PropositionFactory::getInstance()->
			getProposition(*goalItr);
		goals.push_back(prop);
	}
	return goals;
}

// PDDLState PDDLStateFactory::getPDDLState(const MinimalState & state,
// 		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic) {
// 	std::set<PDDLObject> objectSymbolTable = this->objectParameterTable;

// 	std::list<Proposition> propositions = PDDLStateFactory::getPropositions(
// 			state, objectSymbolTable);
// 	std::list<PNE> pnes = getPNEs(state, objectSymbolTable);
// 	std::list<TIL> tils = getTILs(state, timestamp, objectSymbolTable);
// 	std::list<PendingAction> pendingActions = getPendingActions(state,
// 			timestamp, objectSymbolTable);
// 	addRequiredPropositionsForPendingActions(pendingActions, propositions);
// 	std::list<string> planPrefix = getPlanPrefix(plan);
// 	return PDDLState(objectSymbolTable, propositions, pnes, tils,
// 			pendingActions, goals, metric, planPrefix, heuristic, timestamp);
// }

PDDLState PDDLStateFactory::getDeTILedPDDLState(
			const Planner::MinimalState & state,
			std::list<Planner::FFEvent>& plan, double timestamp, double heuristic,
			std::list<PDDL::Proposition> & tilPredicates, 
			std::list<PDDL::Proposition> & tilRequiredObjects,
			std::set<PDDLObject> & tilObjectSymbolTable){
			std::set<PDDLObject> objectSymbolTable;
	//Insert objects found from reading static data
	objectSymbolTable.insert(this->objectParameterTable.begin(),
			this->objectParameterTable.end());
	//Insert objects found from TILs
	objectSymbolTable.insert(tilObjectSymbolTable.begin(),
			tilObjectSymbolTable.end());

	std::list<Proposition> propositions = PDDLStateFactory::getPropositions(
			state, objectSymbolTable);
	std::list<PNE> pnes = getPNEs(state, objectSymbolTable);
	std::list<PendingAction> pendingActions = getPendingActions(state,
			timestamp, objectSymbolTable);
		
	addRequiredPropositionsForPendingActions(pendingActions, propositions);
	addTILPropositions(tilRequiredObjects, tilPredicates, propositions);

	std::list<string> planPrefix = getPlanPrefix(plan);
	PDDLState theState(objectSymbolTable, propositions, tilPredicates, pnes, 
		pendingActions, goals, metric, planPrefix, heuristic, timestamp);

	return theState;
}

/**
 * Cycles through each pending action, and then creates the
 * required propositions
 */
void PDDLStateFactory::addRequiredPropositionsForPendingActions(
		const std::list<PendingAction> & pendingActions,
		std::list<Proposition> & propositions) {
	std::list<PendingAction>::const_iterator pActItr = pendingActions.begin();
	for (; pActItr != pendingActions.end(); pActItr++) {
		std::list<Proposition> extraProps = getRequiredPropositions(
				pActItr->getParameters(), pActItr->getName());
		propositions.insert(propositions.end(), extraProps.begin(),
				extraProps.end());
	}
}

/**
 * Creates all the required propositions for the pending actions
 * This ensures the correct objects get used for the actions
 */
std::list<Proposition> PDDLStateFactory::getRequiredPropositions(
		const std::set<PDDLObject> & parameters, std::string actionName) {
	std::list<Proposition> requiredProps;
	std::string actionDelim = "-" + actionName;
	std::set<PDDLObject>::const_iterator paramItr = parameters.begin();
	for (; paramItr != parameters.end(); paramItr++) {
		list<string> args;
		args.push_back(paramItr->getName());
		Proposition prop(PDDLDomainFactory::REQUIRED_PROPOSITION + actionDelim,
				args);
		requiredProps.push_back(prop);
	}
	return requiredProps;
}

std::list<PDDL::Proposition> PDDLStateFactory::getPropositions(
		const Planner::MinimalState & state,
		std::set<PDDLObject> & objectSymbolTable) {
	std::list<PDDL::Proposition> literals;
	//Cycle through State Literal Facts
	const StateFacts & stateFacts = state.first;
	StateFacts::const_iterator cfItr = stateFacts.begin();
	const StateFacts::const_iterator cfEnd = stateFacts.end();
	for (; cfItr != cfEnd; cfItr++) {
		int literalID = *cfItr;
		Inst::Literal * aliteral = RPGBuilder::getLiteral(literalID);
		PDDL::extractParameters(aliteral, objectSymbolTable, constants);
		PDDL::Proposition literal = PropositionFactory::getInstance()->
			getProposition(aliteral);
		literals.push_back(literal);
	}
	//Insert static propositions
	literals.insert(literals.end(), staticPropositions.begin(),
			staticPropositions.end());

	return literals;
}

/**
 * FIXME: Does not generate parameter table for propositions created from actual PDDL Parse Tree
 */
std::list<PDDL::Proposition> PDDLStateFactory::getStaticPropositions(
		std::list<PDDL::Proposition> & dynamicLiterals,
		std::set<PDDLObject> & objectSymbolTable) {
	std::list<PDDL::Proposition> staticLiterals;

	//Look for static literals, these are usually new and unique, no need for checks
	std::vector<pair<bool, bool> > modelledStaticLiterals =
			Planner::RPGBuilder::getStaticLiterals();
	std::vector<pair<bool, bool> >::const_iterator slItr =
			modelledStaticLiterals.begin();
	const std::vector<pair<bool, bool> >::const_iterator slItrEnd =
			modelledStaticLiterals.end();
	int i = 0;
	for (; slItr != slItrEnd; slItr++) {
		Inst::Literal * aliteral = RPGBuilder::getLiteral(i++);
		if (slItr->first && slItr->second) {
			PDDL::extractParameters(aliteral, objectSymbolTable, constants);
			PDDL::Proposition literal = PropositionFactory::getInstance()->
				getProposition(aliteral);
			staticLiterals.push_back(literal);
		}
	}

	/*Look for all other literals, these are ones missed by all
	 * other checks and requires going back to the original
	 * state description. These will require duplications checks.*/

	VAL::pc_list<VAL::simple_effect*> & props =
			VAL::current_analysis->the_problem->initial_state->add_effects;
	VAL::pc_list<VAL::simple_effect*>::const_iterator propItr = props.begin();
	for (; propItr != props.end(); propItr++) {
		VAL::simple_effect* prop = *propItr;
		std::string name = prop->prop->head->getName();
		std::list<std::string> args;
		VAL::typed_symbol_list<VAL::parameter_symbol>::const_iterator argItr =
				prop->prop->args->begin();
		for (; argItr != prop->prop->args->end(); argItr++) {
			args.push_back((*argItr)->getName());
		}
		PDDL::Proposition prop2(name, args);
		if ((std::find(dynamicLiterals.begin(), dynamicLiterals.end(), prop2)
				== dynamicLiterals.end())
				&& (std::find(staticLiterals.begin(), staticLiterals.end(),
						prop2) == staticLiterals.end())) {
			
			PDDL::extractParameters(prop, objectSymbolTable, constants);
			staticLiterals.push_back(prop2);
		}
	}
	return staticLiterals;
}

std::list<PDDL::PNE> PDDLStateFactory::getPNEs(
		const Planner::MinimalState & state,
		std::set<PDDLObject> & objectSymbolTable) {
	std::list<PDDL::PNE> pnes;
	//Cycle through PNEs
	const int pneCount = state.secondMin.size();
	for (int i = 0; i < pneCount; i++) {
		Inst::PNE* aPNE = Planner::RPGBuilder::getPNE(i);

		PDDL::extractParameters(aPNE, objectSymbolTable, constants);
		PDDL::PNE pne = PDDL::getPNE(aPNE, state.secondMin[i]);
		pnes.push_back(pne);
	}

	pnes.insert(pnes.end(), staticPNEs.begin(), staticPNEs.end());
	return pnes;
}

/**
 * FIXME: Does not generate parameter table for propositions created from actual PDDL Parse Tree
 */
std::list<PDDL::PNE> PDDLStateFactory::getStaticPNEs(
		std::list<PDDL::PNE> dynamicPNEs) {
	std::list<PDDL::PNE> staticPNEs;
	/*Look for all other literals, these are ones missed by all
	 * other checks and requires going back to the original
	 * state description. These will require duplications checks.*/

	VAL::pc_list<VAL::assignment*> & pnes =
			VAL::current_analysis->the_problem->initial_state->assign_effects;
	VAL::pc_list<VAL::assignment*>::const_iterator pneItr = pnes.begin();
	for (; pneItr != pnes.end(); pneItr++) {
		VAL::assignment* pne = *pneItr;
		std::string name = pne->getFTerm()->getFunction()->getName();
		std::list<std::string> args;
		VAL::typed_symbol_list<VAL::parameter_symbol>::const_iterator argItr =
				pne->getFTerm()->getArgs()->begin();
		for (; argItr != pne->getFTerm()->getArgs()->end(); argItr++) {
			args.push_back((*argItr)->getName());
		}
		double value = ((VAL::num_expression*) pne->getExpr())->double_value();
		PDDL::PNE pne2(name, args, value);

		if ((std::find(dynamicPNEs.begin(), dynamicPNEs.end(), pne2)
				== dynamicPNEs.end())
				&& (std::find(staticPNEs.begin(), staticPNEs.end(), pne2)
						== staticPNEs.end())) {
			staticPNEs.push_back(pne2);
		}
	}
	return staticPNEs;

}

/**
 * Cycles through each TIL, and then creates the
 * required propositions
 */
void PDDLStateFactory::addTILPropositions(
		const std::list<PDDL::Proposition> & requiredObjects,
		const std::list<PDDL::Proposition> & tilPredicates,
		std::list<Proposition> & propositions) {
	//TIL Object Required Predicates
	propositions.insert(propositions.end(), requiredObjects.begin(),
				requiredObjects.end());
	//TIL Achieved Predicates
	propositions.insert(propositions.end(), tilPredicates.begin(),
				tilPredicates.end());
}

/**
 * Get the actions in a state that are in the middle of executing.
 * For example the start snap action has been executed, but not the end snap action
 */
std::list<PDDL::PendingAction> PDDLStateFactory::getPendingActions(
		const Planner::MinimalState & state, double timestamp,
		std::set<PDDLObject> & objectSymbolTable) {
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
			parameters = extractParameters(*addItr, parameters, constants);
			Proposition prop = PropositionFactory::getInstance()->
				getProposition(*addItr);
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
			parameters = extractParameters(*delItr, parameters, constants);
			Proposition prop = PropositionFactory::getInstance()->
				getProposition(*delItr);
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
			parameters = extractParameters(aPNE, parameters, constants);
		}
		// insert all parameters into master table
		objectSymbolTable.insert(parameters.begin(), parameters.end());
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
		//This is required to ensure the parameters are created
		PDDL::extractParameters(*condItr, parameters, constants);
		PDDL::Proposition literal = PropositionFactory::getInstance()->
			getProposition(*condItr);
		conditions.push_back(
				pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> >(
						literal,
						std::pair<VAL::time_spec, bool>(timeQualifier,
								isPositive)));
	}
	return conditions;
}

}
