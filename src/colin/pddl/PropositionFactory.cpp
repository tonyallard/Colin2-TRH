/*
 * PropositionFactory.cpp
 *
 *  Created on: 29 Apr 2016
 *      Author: tony
 */

#include "PropositionFactory.h"
#include "Proposition.h"
#include "PDDLUtils.h"

#include "ptree.h"

using namespace std;

namespace PDDL {

PropositionFactory * PropositionFactory::INSTANCE = NULL;

PropositionFactory * PropositionFactory::getInstance() {
	if (!INSTANCE) {
		INSTANCE = new PropositionFactory();
	}
	return INSTANCE;
}

list<PDDL::Proposition> PropositionFactory::getPropositions(
		const list<Inst::Literal*> * literals,
		bool isTemplate /* = false */, bool showType /*= false*/) {
	list<PDDL::Proposition> pddlLiterals;
	list<Inst::Literal*>::const_iterator litItr = literals->begin();
	const list<Inst::Literal*>::const_iterator litItrEnd = literals->end();
	for (; litItr != litItrEnd; litItr++) {
		PDDL::Proposition lit = getProposition(*litItr, isTemplate, showType);
		pddlLiterals.push_back(lit);
	}
	return pddlLiterals;
}

PDDL::Proposition PropositionFactory::getProposition(const Inst::Literal * aLiteral,
		bool isTemplate /* = false */, bool showType /*= false*/) {
	return getProposition(aLiteral->getProp(), isTemplate, showType);
}

/**
 * Almost the same as VAL::proposition, except the arguments must be variables 
 */
PDDL::Proposition PropositionFactory::getProposition(const VAL::pred_decl * predicate) {
	string name = predicate->getPred()->getName();
	transform(name.begin(), name.end(), name.begin(), ::toupper);
	list<string> variables = getParameters(predicate->getArgs(), true, true);
	return PDDL::Proposition(name, variables);
}

PDDL::Proposition PropositionFactory::getProposition(const VAL::proposition * prop, bool isTemplate, bool showType) {
	string name = prop->head->getName();
	transform(name.begin(), name.end(), name.begin(), ::toupper);
	list<string> parameters = getParameters(prop->args, isTemplate, showType);
	return PDDL::Proposition(name, parameters);
}

list<string> PropositionFactory::getParameters(
	const VAL::parameter_symbol_list * params, bool isTemplate /* = false */, bool showType /*= false*/) {
	
	list<string> parameters;
	VAL::parameter_symbol_list::const_iterator argItr =
			params->begin();
	const VAL::parameter_symbol_list::const_iterator argItrEnd =
			params->end();
	for (; argItr != argItrEnd; argItr++) {
		parameters.push_back(getParameter(*argItr, isTemplate, showType));
	}
	return parameters;
}

list<string> PropositionFactory::getParameters(
	const VAL::var_symbol_list * params, bool isTemplate /* = false */, bool showType /*= false*/) {
	
	list<string> parameters;
	VAL::var_symbol_list::const_iterator argItr =
			params->begin();
	const VAL::var_symbol_list::const_iterator argItrEnd =
			params->end();
	for (; argItr != argItrEnd; argItr++) {
		parameters.push_back(getParameter(*argItr, isTemplate, showType));
	}
	return parameters;
}

string PropositionFactory::getParameter(VAL::pddl_typed_symbol * symbol, bool isTemplate, bool showType) {
	string argName = symbol->getName();
	transform(argName.begin(), argName.end(), argName.begin(), ::toupper);
	//if this is a variable argument
	if (isTemplate) {
		argName = "?" + argName;
	}
	if (showType) {
		argName += " - " + PDDL::getPDDLTypeString(symbol);
	}
	return argName;
}

}