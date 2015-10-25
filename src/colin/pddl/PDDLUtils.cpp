/*
 * PDDLUtils.cpp
 *
 *  Created on: 25 Aug 2015
 *      Author: tony
 */

#include <sstream>
#include <string>
#include <list>

#include "PDDLObject.h"
#include "PDDLUtils.h"
#include "../RPGBuilder.h"

using namespace std;

namespace PDDL {

//Literal and PNE Helper Functions
set<PDDLObject> & extractParameters(
		Inst::Literal * literal, set<PDDLObject> & parameters) {


	VAL::parameter_symbol_list::const_iterator argItr =
			literal->toProposition()->args->begin();
	const VAL::parameter_symbol_list::const_iterator argItrEnd =
			literal->toProposition()->args->end();

	for (; argItr != argItrEnd; argItr++) {
		const VAL::parameter_symbol * param = *argItr;
		string paramName = param->getName();
		string paramType = param->type->getName();
		std::transform(paramType.begin(), paramType.end(), paramType.begin(),
						::toupper);
		PDDLObject pddlObj(paramName, paramType);
		parameters.insert(pddlObj);
	}
	return parameters;
}

set<PDDLObject> & extractParameters(
		Inst::PNE * pne, set<PDDLObject> & parameters) {


	VAL::parameter_symbol_list::const_iterator argItr =
			pne->getFunc()->getArgs()->begin();
	const VAL::parameter_symbol_list::const_iterator argItrEnd =
			pne->getFunc()->getArgs()->end();

	for (; argItr != argItrEnd; argItr++) {
		const VAL::parameter_symbol * param = *argItr;
		string paramName = param->getName();
		string paramType = param->type->getName();
		std::transform(paramType.begin(), paramType.end(), paramType.begin(),
						::toupper);
		PDDLObject pddlObj(paramName, paramType);
		parameters.insert(pddlObj);
	}
	return parameters;
}

//Action Helper Functions

std::list<PDDL::Proposition> getActionEffects(int actionID,
		VAL::time_spec timeQualifier, bool positive) {
	std::list<Inst::Literal*> effects;

	//Depending on time spec (start or end) add required effects
	if (timeQualifier == VAL::time_spec::E_AT_START) {
		if (positive) {
			effects = Planner::RPGBuilder::getStartPropositionAdds()[actionID];
		} else {
			effects =
					Planner::RPGBuilder::getStartPropositionDeletes()[actionID];
		}
	} else if (timeQualifier == VAL::time_spec::E_AT_END) {
		if (positive) {
			effects = Planner::RPGBuilder::getEndPropositionAdds()[actionID];
		} else {
			effects = Planner::RPGBuilder::getEndPropositionDeletes()[actionID];
		}
	} else {
		std::cerr << "This case not catered for.";
	}
	return getPropositions(&effects);
}

std::list<PDDL::Literal> getActionConditions(int actionID,
		VAL::time_spec timeQualifier) {
	std::list<Inst::Literal*> positiveConditions;
	std::list<Inst::Literal*> negativeConditions;
	//Add invariant (over all) conditions regardless
	//Positive First
	std::list<Inst::Literal*> tmpConditions =
			Planner::RPGBuilder::getInvariantPropositionalPreconditions()[actionID];
	positiveConditions.insert(positiveConditions.end(), tmpConditions.begin(),
			tmpConditions.end());
	//Then Negative
	tmpConditions =
			Planner::RPGBuilder::getInvariantNegativePropositionalPreconditions()[actionID];
	negativeConditions.insert(negativeConditions.end(), tmpConditions.begin(),
			tmpConditions.end());

	//Depending on time spec (start or end) add required conditions

	if (timeQualifier == VAL::time_spec::E_AT_START) {
		//Positive First
		tmpConditions =
				Planner::RPGBuilder::getStartPropositionalPreconditions()[actionID];
		positiveConditions.insert(positiveConditions.end(),
				tmpConditions.begin(), tmpConditions.end());
		//Then Negative
		tmpConditions =
				Planner::RPGBuilder::getStartNegativePropositionalPreconditions()[actionID];
		negativeConditions.insert(negativeConditions.end(),
				tmpConditions.begin(), tmpConditions.end());
	} else if (timeQualifier == VAL::time_spec::E_AT_END) {
		//Positive First
		tmpConditions =
				Planner::RPGBuilder::getEndPropositionalPreconditions()[actionID];
		positiveConditions.insert(positiveConditions.end(),
				tmpConditions.begin(), tmpConditions.end());
		//Then Negative
		tmpConditions =
				Planner::RPGBuilder::getEndNegativePropositionalPreconditions()[actionID];
		negativeConditions.insert(negativeConditions.end(),
				tmpConditions.begin(), tmpConditions.end());
	} else {
		std::cerr << "This case not catered for.";
	}

	std::list<PDDL::Literal> literals;
	std::list<PDDL::Literal> tmpLiterals = PDDL::getLiterals(
			&positiveConditions, true);
	literals.insert(literals.end(), tmpLiterals.begin(), tmpLiterals.end());
	tmpLiterals = PDDL::getLiterals(&negativeConditions, false);
	literals.insert(literals.end(), tmpLiterals.begin(), tmpLiterals.end());
	return literals;
}

std::string getActionName(int actionNum) {
	std::ostringstream output;
	Inst::instantiatedOp* action = Planner::RPGBuilder::getInstantiatedOp(
			actionNum);
	output << action->getHead()->getName();
	VAL::var_symbol_list::const_iterator paramItr =
			action->forOp()->parameters->begin();
	const VAL::var_symbol_list::const_iterator paramItrEnd =
			action->forOp()->parameters->end();
	for (; paramItr != paramItrEnd; paramItr++) {
		output << "-" << ((*action->getEnv())[*paramItr])->getName();
	}
	return output.str();
}

std::map<std::string, std::string> getActionParameters(int actionNum) {
	std::map<std::string, std::string> output;
	Inst::instantiatedOp* action = Planner::RPGBuilder::getInstantiatedOp(
			actionNum);
	VAL::FastEnvironment::const_iterator paramItr = action->getEnv()->begin();
	char letter = 'a';
	for (; paramItr != action->getEnv()->end(); paramItr++) {
		ostringstream var;
		var << "?" << static_cast<char>(letter);
		string varType = (*paramItr)->type->getName();
		std::transform(varType.begin(), varType.end(), varType.begin(),
				::toupper);
		output[var.str()] = varType;
		letter++;
	}
	return output;
}

bool supported(const PDDL::Proposition * proposition,
		std::list<PDDL::Proposition> * propositions) {
	std::list<PDDL::Proposition>::iterator propItr = propositions->begin();
	const std::list<PDDL::Proposition>::iterator propItrEnd =
			propositions->end();
	for (; propItr != propItrEnd; propItr++) {
		if ((*propItr) == (*proposition)) {
			return true;
		}
	}
	return false;
}

bool isEqual(const Planner::FFEvent * one, const Planner::FFEvent * two) {

}

// Basic Conversions Functions

std::list<PDDL::Proposition> getPropositions(
		std::list<Inst::Literal*> * literals) {
	std::list<PDDL::Proposition> pddlLiterals;
	std::list<Inst::Literal*>::const_iterator litItr = literals->begin();
	const std::list<Inst::Literal*>::const_iterator litItrEnd = literals->end();
	for (; litItr != litItrEnd; litItr++) {
		PDDL::Proposition lit = getProposition(*litItr);
		pddlLiterals.push_back(lit);
	}
	return pddlLiterals;
}

PDDL::Proposition getProposition(const Inst::Literal * aLiteral) {
	std::string name = aLiteral->getProp()->head->getName();
	std::list<std::string> variables;
	VAL::parameter_symbol_list::const_iterator argItr =
			aLiteral->getProp()->args->begin();
	const VAL::parameter_symbol_list::const_iterator argItrEnd =
			aLiteral->getProp()->args->end();
	for (; argItr != argItrEnd; argItr++) {
		variables.push_back(((*aLiteral->getEnv())[*argItr])->getName());
	}
	return PDDL::Proposition(name, variables);
}

std::list<PDDL::Literal> getLiterals(std::list<Inst::Literal*> * propositions,
		bool positive) {
	std::list<PDDL::Literal> literals;
	std::list<Inst::Literal*>::const_iterator propItr = propositions->begin();
	const std::list<Inst::Literal*>::const_iterator propItrEnd =
			propositions->end();

	for (; propItr != propItrEnd; propItr++) {
		PDDL::Literal literal = getLiteral(*propItr, positive);
		literals.push_back(literal);
	}
	return literals;
}

PDDL::Literal getLiteral(const Inst::Literal * aProposition, bool positive) {
	PDDL::Proposition proposition = PDDL::getProposition(aProposition);
	return PDDL::Literal(proposition, positive);
}

PDDL::PNE getPNE(const Inst::PNE * aPNE, double value) {
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

PDDL::TIL getTIL(Planner::FakeTILAction aTIL, double aTimestamp) {
	double timestamp = aTIL.duration - aTimestamp;
	std::list<PDDL::Proposition> addEffects;
	std::list<PDDL::Proposition> delEffects;

	//Get Add effects
	std::list<Inst::Literal*>::const_iterator tilAddLitInt =
			aTIL.addEffects.begin();
	std::list<Inst::Literal*>::const_iterator tilAddLitIntEnd =
			aTIL.addEffects.end();
	for (; tilAddLitInt != tilAddLitIntEnd; tilAddLitInt++) {
		Inst::Literal * literal = (*tilAddLitInt);
		PDDL::Proposition lit = PDDL::getProposition(literal);
		addEffects.push_back(lit);
	}

	//Get Del Effects
	std::list<Inst::Literal*>::const_iterator tilDelLitInt =
			aTIL.delEffects.begin();
	std::list<Inst::Literal*>::const_iterator tilDelLitIntEnd =
			aTIL.delEffects.end();
	for (; tilDelLitInt != tilDelLitIntEnd; tilDelLitInt++) {
		Inst::Literal * literal = (*tilDelLitInt);
		PDDL::Proposition lit = PDDL::getProposition(literal);
		delEffects.push_back(lit);
	}
	return PDDL::TIL(addEffects, delEffects, timestamp);
}

PDDL::PendingProposition getPendingProposition(const Inst::Literal * aLiteral,
		std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions,
		double timestamp, bool addEffect) {
	PDDL::Proposition literal = PDDL::getProposition(aLiteral);
	PendingProposition pendingLiteral(literal.getName(), literal.getArguments(),
			conditions, timestamp, addEffect);
	return pendingLiteral;
}

//Plan Helper Functions

std::list<std::string> getPlanPrefix(std::list<Planner::FFEvent>& plan) {
	std::list<std::string> prefix;
	std::list<Planner::FFEvent>::const_iterator eventItr = plan.begin();
	const std::list<Planner::FFEvent>::const_iterator eventItrEnd = plan.end();
	for (; eventItr != eventItrEnd; eventItr++) {
		ostringstream name;
		name << (*eventItr).lpTimestamp << ": "
				<< PDDL::getActionName((*eventItr).action->getID());
		prefix.push_back(name.str());
	}
	return prefix;
}

bool isAfter(const Planner::FFEvent * event, const Planner::FFEvent * after,
		std::list<Planner::FFEvent> * plan) {
	bool foundEvent = false;
	std::list<Planner::FFEvent>::reverse_iterator eventItr = plan->rbegin();
	const std::list<Planner::FFEvent>::reverse_iterator eventItrEnd =
			plan->rend();
	for (; eventItr != eventItrEnd; eventItr++) {
		const Planner::FFEvent * ev = &(*eventItr);
		if (ev == event) {
			foundEvent = true;
		} else if (ev == after) {
			return foundEvent;
		}
	}
	return false;
}
bool isBefore(const Planner::FFEvent * event, const Planner::FFEvent * before,
		std::list<Planner::FFEvent> * plan) {
	bool foundEvent = false;
	std::list<Planner::FFEvent>::const_iterator eventItr = plan->begin();
	const std::list<Planner::FFEvent>::const_iterator eventItrEnd = plan->end();
	for (; eventItr != eventItrEnd; eventItr++) {
		const Planner::FFEvent * ev = &(*eventItr);
		if (ev == event) {
			foundEvent = true;
		} else if (ev == before) {
			return foundEvent;
		}
	}
	return false;
}

std::list<const Planner::FFEvent *> getTILActions(
		std::list<Planner::FFEvent> * plan) {
	std::list<const Planner::FFEvent *> tilActions;

	std::list<Planner::FFEvent>::const_iterator eventItr = plan->begin();
	const std::list<Planner::FFEvent>::const_iterator eventItrEnd = plan->end();

	for (; eventItr != eventItrEnd; eventItr++) {
		const Planner::FFEvent * event = &(*eventItr);
		if (event->maxDuration == event->minDuration == 0) {
			tilActions.push_back(event);
		}
	}

	return tilActions;
}

}
