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
#include "FakeTILAction.h"
#include "instantiation.h"
#include "Utils.h"

using namespace Planner;

namespace PDDL {

PDDLState PDDLStateFactory::getPDDLState(const MinimalState & state,
		double timestamp, double heuristic) {
	std::list<Literal> literals = getLiterals(state);
	std::list<PNE> pnes = getPNEs(state);
	std::list<TIL> tils = getTILs(state, timestamp);
//	std::list<PendingLiteral> pendingLiterals = getPendingLiterals(state,
//			timestamp);
//	std::list<PendingPNE> pendingPNEs = getPendingPNEs(state, timestamp);
	std::list<PendingAction> pendingActions = getPendingActions(state, timestamp);
	return PDDLState(literals, pnes, tils, heuristic, timestamp);
}

std::list<PDDL::Literal> PDDLStateFactory::getLiterals(
		const Planner::MinimalState & state) {
	std::list<PDDL::Literal> literals;
	//Cycle through State Literal Facts
	const StateFacts & stateFacts = state.first;
	StateFacts::const_iterator cfItr = stateFacts.begin();
	const StateFacts::const_iterator cfEnd = stateFacts.end();
	for (; cfItr != cfEnd; cfItr++) {
		int stateFact = *cfItr;
		Inst::Literal * aliteral = RPGBuilder::getLiteral(stateFact);
		PDDL::Literal literal = PDDLStateFactory::getLiteral(aliteral);
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
			PDDL::Literal literal = PDDLStateFactory::getLiteral(aliteral);
			literals.push_back(literal);
		}
	}
	return literals;
}

PDDL::Literal PDDLStateFactory::getLiteral(const Inst::Literal * aLiteral) {
	string name = aLiteral->getProp()->head->getName();
	std::list<string> variables;
	VAL::parameter_symbol_list::const_iterator argItr =
			aLiteral->getProp()->args->begin();
	const VAL::parameter_symbol_list::const_iterator argItrEnd =
			aLiteral->getProp()->args->end();
	for (; argItr != argItrEnd; argItr++) {
		variables.push_back(((*aLiteral->getEnv())[*argItr])->getName());
	}
	return PDDL::Literal(name, variables);
}

std::list<PDDL::PNE> PDDLStateFactory::getPNEs(
		const Planner::MinimalState & state) {
	std::list<PDDL::PNE> pnes;
	//Cycle through PNEs
	const int pneCount = state.secondMin.size();
	for (int i = 0; i < pneCount; i++) {
		Inst::PNE* aPNE = Planner::RPGBuilder::getPNE(i);
		PDDL::PNE pne = PDDLStateFactory::getPNE(aPNE, state.secondMin[i]);
		pnes.push_back(pne);
	}

	//Manually Add static PNEs
	pnes.push_back(PNE("travel-time", { "V1", "L2", "L1" }, 2.0));
	pnes.push_back(PNE("travel-time", { "V1", "L1", "L2" }, 2.0));
	pnes.push_back(PNE("travel-time", { "V2", "L2", "L3" }, 2.0));
	pnes.push_back(PNE("travel-time", { "V2", "L3", "L2" }, 2.0));
	pnes.push_back(PNE("size", { "C1" }, 1.0));
	pnes.push_back(PNE("load-time", { "V1", "L1" }, 1.0));
	pnes.push_back(PNE("load-time", { "V1", "L2" }, 1.0));
	pnes.push_back(PNE("unload-time", { "V1", "L1" }, 1.0));
	pnes.push_back(PNE("unload-time", { "V1", "L2" }, 1.0));
	pnes.push_back(PNE("load-time", { "V2", "L3" }, 1.0));
	pnes.push_back(PNE("load-time", { "V2", "L2" }, 1.0));
	pnes.push_back(PNE("unload-time", { "V2", "L3" }, 1.0));
	pnes.push_back(PNE("unload-time", { "V2", "L2" }, 1.0));
	pnes.push_back(PNE("cost", { "V1" }, 1.0));
	pnes.push_back(PNE("cost", { "V2" }, 1.0));

	return pnes;
}

PDDL::PNE PDDLStateFactory::getPNE(const Inst::PNE * aPNE, double value) {
	string name = aPNE->getFunc()->getFunction()->getName();
	std::list<string> arguments;
	VAL::parameter_symbol_list::const_iterator argItr =
			aPNE->getFunc()->getArgs()->begin();
	const VAL::parameter_symbol_list::const_iterator argItrEnd =
			aPNE->getFunc()->getArgs()->end();
	for (; argItr != argItrEnd; argItr++) {
		arguments.push_back((*argItr)->getName());
	}
	return PDDL::PNE(name, arguments, value);
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
		PDDL::TIL til = PDDLStateFactory::getTIL(*tilItr, timestamp);
		tils.push_back(til);
	}
	return tils;
}

PDDL::TIL PDDLStateFactory::getTIL(Planner::FakeTILAction aTIL,
		double aTimestamp) {
	double timestamp = aTIL.duration - aTimestamp;
	std::list<PDDL::Literal> addEffects;
	std::list<PDDL::Literal> delEffects;

	//Get Add effects
	std::list<Inst::Literal*>::const_iterator tilAddLitInt =
			aTIL.addEffects.begin();
	std::list<Inst::Literal*>::const_iterator tilAddLitIntEnd =
			aTIL.addEffects.end();
	for (; tilAddLitInt != tilAddLitIntEnd; tilAddLitInt++) {
		Inst::Literal * literal = (*tilAddLitInt);
		PDDL::Literal lit = PDDLStateFactory::getLiteral(literal);
		addEffects.push_back(lit);
	}

	//Get Del Effects
	std::list<Inst::Literal*>::const_iterator tilDelLitInt =
			aTIL.delEffects.begin();
	std::list<Inst::Literal*>::const_iterator tilDelLitIntEnd =
			aTIL.delEffects.end();
	for (; tilDelLitInt != tilDelLitIntEnd; tilDelLitInt++) {
		Inst::Literal * literal = (*tilDelLitInt);
		PDDL::Literal lit = PDDLStateFactory::getLiteral(literal);
		delEffects.push_back(lit);
	}
	return PDDL::TIL(addEffects, delEffects, timestamp);
}

std::list<PendingLiteral> PDDLStateFactory::getPendingLiterals(
		const Planner::MinimalState & state, double timestamp) {
	std::list<PendingLiteral> pendingLiterals;
	//Cycle through Facts held up by executing actions (these are effects coming into play)
	std::map<int, std::set<int> >::const_iterator saItr =
			state.startedActions.begin();
	const std::map<int, std::set<int> >::const_iterator saItrEnd =
			state.startedActions.end();
	for (; saItr != saItrEnd; saItr++) {
		//For each action get its conditions
		/*Postconditions*/
		std::list<pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions;

		std::list<pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > tempConditions =
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
			PendingLiteral pendingLiteral = PDDLStateFactory::getPendingLiteral(
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
			PendingLiteral pendingLiteral = PDDLStateFactory::getPendingLiteral(
					*addItr, conditions, (*saItr->second.begin() - timestamp),
					false);
			pendingLiterals.push_back(pendingLiteral);
		}
	}
	return pendingLiterals;
}

PDDL::PendingLiteral PDDLStateFactory::getPendingLiteral(
		const Inst::Literal * aLiteral,
		std::list<std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions,
		double timestamp, bool addEffect) {
	PDDL::Literal literal = PDDLStateFactory::getLiteral(aLiteral);
	PendingLiteral pendingLiteral(literal.getName(), literal.getArguments(),
			conditions, timestamp, addEffect);
	return pendingLiteral;
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
		std::list<pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions;

		std::list<pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > tempConditions =
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
		std::list<std::pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions,
		double minDur, double maxDur) {
	//Get the numeric effect information
	RPGBuilder::RPGNumericEffect & effect = RPGBuilder::getNumericEff()[numeric];
	//Apply the numeric effect to get new fluent values
	pair<double, double> result = effect.applyEffectMinMax(state.secondMin,
			state.secondMax, minDur, maxDur);
	//Determine the PNE that the values belong
	Inst::PNE* aPNE = Planner::RPGBuilder::getPNE(effect.fluentIndex);
	PDDL::PNE pne = PDDLStateFactory::getPNE(aPNE, result.first);
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
		std::list<pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions;

		std::list<pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > tempConditions =
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
		std::list<PDDL::Literal> literalAddEffects;
		std::list<Inst::Literal*> adds =
				RPGBuilder::getEndPropositionAdds()[saItr->first];
		std::list<Inst::Literal*>::const_iterator addItr = adds.begin();
		const std::list<Inst::Literal*>::const_iterator addItrEnd = adds.end();
		for (; addItr != addItrEnd; addItr++) {
			Literal lit = getLiteral(*addItr);
			literalAddEffects.push_back(lit);
		}

		// Literals Deleted by Action
		std::list<PDDL::Literal> literalDelEffects;
		std::list<Inst::Literal*> deletes =
				RPGBuilder::getEndPropositionDeletes()[saItr->first];
		std::list<Inst::Literal*>::const_iterator delItr = deletes.begin();
		const std::list<Inst::Literal*>::const_iterator delItrEnd =
				deletes.end();
		for (; delItr != delItrEnd; delItr++) {
			Literal lit = getLiteral(*delItr);
			literalDelEffects.push_back(lit);
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
			PDDL::PNE pne = PDDLStateFactory::getPNE(aPNE, result.first);
			pneEffects.push_back(pne);
		}
		PendingAction pendingAction ("action" + VAL::toString(saItr->first), literalAddEffects, literalDelEffects, pneEffects, conditions, minDur);
		pendingActions.push_back(pendingAction);
		cout << pendingAction;
	}
	return pendingActions;
}

std::list<pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > PDDLStateFactory::getConditions(
		std::list<Inst::Literal*> conditionLiterals,
		VAL::time_spec timeQualifier, bool isPositive) {
	std::list<pair<PDDL::Literal, std::pair<VAL::time_spec, bool> > > conditions;

	std::list<Inst::Literal*>::const_iterator condItr =
			conditionLiterals.begin();
	std::list<Inst::Literal*>::const_iterator condItrEnd =
			conditionLiterals.end();

	for (; condItr != condItrEnd; condItr++) {
		PDDL::Literal literal = PDDLStateFactory::getLiteral(*condItr);
		conditions.push_back(
				pair<PDDL::Literal, std::pair<VAL::time_spec, bool> >(literal,
						std::pair<VAL::time_spec, bool>(timeQualifier,
								isPositive)));
	}
	return conditions;
}

}
