/*
 * PDDLStateFactory.h
 *
 *  Created on: 2 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PROPOSITIONFACTORY_H_
#define COLIN_PROPOSITIONFACTORY_H_

#include <string>
#include <list>

#include "Proposition.h"

#include "ptree.h"
#include "instantiation.h"

namespace PDDL {

class PropositionFactory {
public:

	static PropositionFactory * getInstance();

	std::list<PDDL::Proposition> getPropositions(
		const std::list<Inst::Literal*> * literals);
	PDDL::Proposition getProposition(const Inst::Literal * aLiteral);
	PDDL::Proposition getProposition(const VAL::pred_decl * predicate);
	PDDL::Proposition getProposition(const VAL::proposition * prop);

private:
	//Singleton Instance
	static PropositionFactory * INSTANCE;
	
	//Private constructor
	PropositionFactory(){};
	//Singleton
	PropositionFactory(PropositionFactory const & other) {
	}
	;
	//Singleton
	PropositionFactory& operator=(PropositionFactory const&) {
	}
	;

	//Private members
	list<string> getParameters(
		const VAL::var_symbol_list * params, bool variables = false);
	list<string> getParameters(
		const VAL::parameter_symbol_list * params, bool variables = false);
	string getParameter(VAL::pddl_typed_symbol * symbol, bool variable);
};

}

#endif /* COLIN_PROPOSITIONFACTORY_H_ */
