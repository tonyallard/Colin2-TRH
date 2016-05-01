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

	std::list<PDDL::Proposition> getPropositions(const std::list<Inst::Literal*> * literals,
			bool isTemplate = false, bool showType = false);
	PDDL::Proposition getProposition(const VAL::pred_decl * predicate);
	PDDL::Proposition getProposition(const Inst::Literal * aLiteral,
			bool isTemplate = false, bool showType = false);
	PDDL::Proposition getProposition(const VAL::proposition * prop,
			bool isTemplate = false, bool showType = false);

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
		const VAL::var_symbol_list * params, bool isTemplate = false, bool showType = false);
	list<string> getParameters(
		const VAL::parameter_symbol_list * params, bool isTemplate = false, bool showType = false);
	string getParameter(VAL::pddl_typed_symbol * symbol, bool isTemplate, bool showType);
};

}

#endif /* COLIN_PROPOSITIONFACTORY_H_ */
