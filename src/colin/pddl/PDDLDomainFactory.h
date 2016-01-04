/*
 * PDDLDomainFactory.h
 *
 *  Created on: 18 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PDDL_PDDLDOMAINFACTORY_H_
#define COLIN_PDDL_PDDLDOMAINFACTORY_H_

#include "Proposition.h"
#include "TIL.h"
#include "PendingAction.h"

namespace PDDL {
class PDDLDomainFactory {
private:
	//Singleton Instance
	static PDDLDomainFactory * INSTANCE;
	//Private constructor
	PDDLDomainFactory(const VAL::domain * domain);
	//Singleton
	PDDLDomainFactory(PDDLDomainFactory const & other) {
	}
	;
	PDDLDomainFactory& operator=(PDDLDomainFactory const&) {
	}
	;
	//Instance Vars
	std::string types;
	std::string domainPredicates;
	std::string functions;
	std::list<std::pair<std::string, std::string> > constants;
	std::string domainOperators;

	std::string getHeader(const VAL::domain * domain, bool deTILed);
	std::string getDomainRequirementsString(VAL::pddl_req_flag flags,
			bool deTILed);
	std::string getTypes(const VAL::pddl_type_list * types);
	std::string getPredicates(const std::list<PendingAction> & pendingActions =
			std::list<PendingAction>(),
			const std::list<TIL> & tils = std::list<TIL>());
	std::string getDomainPredicates(const VAL::pred_decl_list * predicates);
	std::string getFunctions(const VAL::func_decl_list * functions);
	std::list<std::pair<std::string, std::string> > getConstantsFromDomain(
			const VAL::const_symbol_list * constants);
	std::string getConstantsString();
	std::string getDomainOperators(const VAL::operator_list * operators);
	std::ostream & getDurativeAction(const VAL::durative_action * action,
			std::ostream & output);
	std::ostream & getAction(const VAL::action * action, std::ostream & output);
	std::string getLoadAction();
	std::string getUnloadAction();
	std::string getMoveAction();
	std::string getInitialAction();
	std::string getdeTILedActions(std::list<TIL> tils);
	std::string getdeTILedAction(const TIL & til,
			std::list<PDDL::Proposition> * tilActionPreconditions);
	std::string getPendingActions(
			const std::list<PendingAction> & pendingActions);
	std::string getTerminationString();
public:
	static PDDLDomainFactory * getInstance();
	static const std::string TIL_ACHIEVED_PROPOSITION;
	static const std::string REQUIRED_PROPOSITION;
	static const std::string INITIAL_ACTION_REQUIRED_PROPOSITION;
	static const std::string INITIAL_ACTION_COMPLETE_PROPOSITION;

	std::string getDomainString(const VAL::domain * domain,
			const std::list<PendingAction> & pendingActions);
	std::string getDeTILedDomainString(const VAL::domain * domain,
			const std::list<TIL> & tils,
			const std::list<PendingAction> & pendingActions);
	std::list<std::pair<std::string, std::string> > getConstants() {
		return constants;
	}
};
}

#endif /* COLIN_PDDL_PDDLDOMAINFACTORY_H_ */
