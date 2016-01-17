/*
 * PDDLDomainFactory.cpp
 *
 *  Created on: 18 Jul 2015
 *      Author: tony
 */

#include <string>
#include <sstream>
#include <algorithm>

#include "ptree.h"

#include "PDDLDomainFactory.h"
#include "TIL.h"
#include "PDDLUtils.h"

using namespace std;
namespace PDDL {

const std::string PDDLDomainFactory::TIL_ACHIEVED_PROPOSITION = "til-achieved";
const std::string PDDLDomainFactory::REQUIRED_PROPOSITION = "required";
const std::string PDDLDomainFactory::INITIAL_ACTION_COMPLETE_PROPOSITION =
		"initial-action-complete";
PDDLDomainFactory * PDDLDomainFactory::INSTANCE = NULL;

PDDLDomainFactory * PDDLDomainFactory::getInstance() {
	if (!INSTANCE) {
		INSTANCE = new PDDLDomainFactory(VAL::current_analysis->the_domain);
	}
	return INSTANCE;
}

PDDLDomainFactory::PDDLDomainFactory(const VAL::domain * domain) {
	types = getTypes(domain->types);
	domainPredicates = getDomainPredicates(domain->predicates);
	functions = getFunctions(domain->functions);
	constants = getConstantsFromDomain(domain->constants);
	domainOperators = getDomainOperators(domain->ops);
}

std::string PDDLDomainFactory::getDomainString(const VAL::domain * domain,
		const std::list<PendingAction> & pendingActions) {
	ostringstream output;
	output << getHeader(domain, false) << types << getPredicates() << functions
			<< getConstantsString() << domainOperators
			<< getPendingActions(pendingActions) << getTerminationString();
	return output.str();
}

std::string PDDLDomainFactory::getDeTILedDomainString(
		const VAL::domain * domain, const std::list<TIL> & tils,
		const std::list<PendingAction> & pendingActions) {
	ostringstream output;
	bool hasTils = tils.size();
	output << getHeader(domain, hasTils) << types
			<< getPredicates(pendingActions, tils) << functions
			<< getConstantsString() << domainOperators << getInitialAction()
			<< getPendingActions(pendingActions);
	if (hasTils) {
		output << getdeTILedActions(tils);
	}
	output << getTerminationString();
	return output.str();
}

std::string PDDLDomainFactory::getHeader(const VAL::domain * domain,
		bool deTILed) {
	ostringstream output;
	output << "(define (domain " << domain->name << ")" << endl;
	string requirements = getDomainRequirementsString(domain->req, deTILed);
	output << "\t(:requirements " << requirements << ")" << std::endl;
	return output.str();
}

std::string PDDLDomainFactory::getDomainRequirementsString(
		VAL::pddl_req_flag flags, bool deTILed) {
	string result;

	if (flags & VAL::E_EQUALITY)
		result += ":equality ";
	if (flags & VAL::E_STRIPS)
		result += ":strips ";
	if (flags & VAL::E_TYPING)
		result += ":typing ";
	if (flags & VAL::E_DISJUNCTIVE_PRECONDS)
		result += ":disjunctive-preconditions ";
	if (flags & VAL::E_EXT_PRECS)
		result += ":existential-preconditions ";
	if (flags & VAL::E_UNIV_PRECS)
		result += ":universal-preconditions ";
	if (flags & VAL::E_COND_EFFS)
		result += ":conditional-effects ";
	if (flags & VAL::E_NFLUENTS)
		result += ":fluents ";
	if (flags & VAL::E_OFLUENTS)
		result += ":object-fluents ";
	if (flags & VAL::E_ACTIONCOSTS)
		result += ":action-costs ";
	if (flags & VAL::E_DURATIVE_ACTIONS)
		result += ":durative-actions ";
	if (flags & VAL::E_DURATION_INEQUALITIES)
		result += ":duration-inequalities ";
	if (flags & VAL::E_CONTINUOUS_EFFECTS)
		result += ":continuous-effects ";
	if (flags & VAL::E_NEGATIVE_PRECONDITIONS)
		result += ":negative-preconditions ";
	if (flags & VAL::E_DERIVED_PREDICATES)
		result += ":derived-predicates ";
	//If deTiled ignore TIL requirement
	if ((flags & VAL::E_TIMED_INITIAL_LITERALS) && !deTILed)
		result += ":timed-initial-literals ";
	if (flags & VAL::E_PREFERENCES)
		result += ":preferences ";
	if (flags & VAL::E_CONSTRAINTS)
		result += ":constraints ";
	if (flags & VAL::E_TIME)
		result += ":time ";
	return result;
}

std::string PDDLDomainFactory::getTypes(const VAL::pddl_type_list * types) {
	ostringstream output;
	if (types) {
		output << "\t(:types" << endl;
		VAL::pddl_type_list::const_iterator typeItr = types->begin();
		for (; typeItr != types->end(); typeItr++) {
			const VAL::pddl_type * type = *typeItr;
			string name = type->getName();
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);
			output << "\t\t" << name << " - " << getPDDLTypeString(type)
					<< endl;
		}
		output << "\t)" << endl;
	}
	return output.str();
}

std::string PDDLDomainFactory::getDomainPredicates(
		const VAL::pred_decl_list * predicates) {
	ostringstream output;
	//get standard domain predicates
	VAL::pred_decl_list::const_iterator predItr = predicates->begin();
	for (; predItr != predicates->end(); predItr++) {
		const VAL::pred_decl * predicate = *predItr;
		output << "\t\t(" << predicate->getPred()->getName() << " ";
		output << getArgumentString(predicate->getArgs());
		output << ")" << endl;
	}
	return output.str();
}

std::string PDDLDomainFactory::getPredicates(
		const std::list<PendingAction> & pendingActions /*=empty list*/,
		const std::list<TIL> & tils /*=empty list*/) {
	ostringstream output;
	output << "\t(:predicates" << endl;
	//Add Domain Predicates
	output << domainPredicates;
	//Add Predicates for de-TILed Actions
	std::list<TIL>::const_iterator tilItr = tils.begin();
	for (; tilItr != tils.end(); tilItr++) {
		output << "\t\t(" << tilItr->getName() << ")\n";
	}
	//Add Required Predicates for Partial Actions
	if (pendingActions.size()) {
		std::list<PendingAction>::const_iterator pendActItr =
				pendingActions.begin();
		for (; pendActItr != pendingActions.end(); pendActItr++) {
			output << "\t\t(" << PDDLDomainFactory::REQUIRED_PROPOSITION << "-"
					<< pendActItr->getName() << " ?x - object)\n";
		}
	}
	//Add Initial Action Predicate
	output << "\t\t(" << PDDLDomainFactory::INITIAL_ACTION_COMPLETE_PROPOSITION
			<< ")\n";
	output << "\t)\n";
	return output.str();
}

std::string PDDLDomainFactory::getFunctions(
		const VAL::func_decl_list * functions) {
	ostringstream output;
	output << "\t(:functions" << endl;
	VAL::func_decl_list::const_iterator funcItr = functions->begin();
	for (; funcItr != functions->end(); funcItr++) {
		const VAL::func_decl * func = *funcItr;
		output << "\t\t(" << func->getFunction()->getName() << " ";
		output << getArgumentString(func->getArgs());
		output << ")" << endl;
	}
	output << "\t)" << endl;
	return output.str();
}

std::list<std::pair<std::string, std::string> > PDDLDomainFactory::getConstantsFromDomain(
		const VAL::const_symbol_list * constants) {
	std::list<std::pair<std::string, std::string> > constantList;
	if (constants) {
		VAL::const_symbol_list::const_iterator constItr = constants->begin();
		for (; constItr != constants->end(); constItr++) {
			const VAL::const_symbol * con = *constItr;
			string name = con->getName();
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);
			std::pair<std::string, std::string> constant(name,
					getPDDLTypeString(con));
			constantList.push_back(constant);
		}
	}
	return constantList;
}
std::string PDDLDomainFactory::getConstantsString() {
	ostringstream output;
	if (constants.size()) {
		output << "\t(:constants" << endl;
		std::list<std::pair<std::string, std::string> >::const_iterator constItr =
				constants.begin();
		for (; constItr != constants.end(); constItr++) {
			std::pair<std::string, std::string> constant = *constItr;
			output << "\t\t" << constant.first << " - " << constant.second
					<< endl;
		}
		output << "\t)" << endl;
	}
	return output.str();
}

std::string PDDLDomainFactory::getDomainOperators(
		const VAL::operator_list * operators) {
	ostringstream output;
	VAL::operator_list::const_iterator opsItr = operators->begin();
	for (; opsItr != operators->end(); opsItr++) {
		const VAL::operator_ * op = *opsItr;
		//Determine type of action
		const VAL::durative_action * dur_action =
				dynamic_cast<const VAL::durative_action *>(op);
		const VAL::action * action = dynamic_cast<const VAL::action *>(op);
		if (dur_action) {
			getDurativeAction(dur_action, output);
		} else {
			getAction(action, output);
		}
	}
	return output.str();
}

std::ostream & PDDLDomainFactory::getDurativeAction(
		const VAL::durative_action * action, std::ostream & output) {
	//get header
	output << "\t(:durative-action " << action->name->getName() << endl;
	//get parameters
	output << "\t\t:parameters (" << getArgumentString(action->parameters)
			<< ")" << endl;
	//get duration
	const VAL::timed_goal * duration =
			dynamic_cast<const VAL::timed_goal *>(action->dur_constraint);
	const VAL::comparison * durationalGoal =
			dynamic_cast<const VAL::comparison *>(duration->getGoal());
	output << "\t\t:duration " << getExpressionString(durationalGoal) << endl;
	//get conditions
	output << "\t\t:condition" << endl << getConditions(action->precondition, true)
			<< endl;
	//get effects
	output << "\t\t:effect" << endl << getEffectsString(action->effects)
			<< endl;
	output << "\t)" << endl;
	return output;
}

std::ostream & PDDLDomainFactory::getAction(const VAL::action * action,
		std::ostream & output) {
	//get header
	output << "\t(:action " << action->name->getName() << endl;
	//get parameters
	output << "\t\t:parameters (" << getArgumentString(action->parameters)
			<< ")" << endl;
	//get preconditions
	output << "\t\t:precondition" << endl << getConditions(action->precondition, false)
			<< endl;
	//get effects
	output << "\t\t:effect" << endl << getEffectsString(action->effects)
			<< endl;
	output << "\t)" << endl;
	return output;
}

std::string PDDLDomainFactory::getConditions(const VAL::goal * goal,
		bool isForDurativeAction) {
	std::ostringstream output;
	const VAL::conj_goal * conjGoal = dynamic_cast<const VAL::conj_goal *>(goal);
	if (conjGoal) {
		output << "\t\t\t(and " << endl;
		if (isForDurativeAction) {
			output << "\t\t\t\t(at start ("
					<< INITIAL_ACTION_COMPLETE_PROPOSITION << "))" << std::endl;
		} else {
			output << "\t\t\t\t(" << INITIAL_ACTION_COMPLETE_PROPOSITION << ")"
					<< std::endl;
		}
		VAL::goal_list::const_iterator goalItr = conjGoal->getGoals()->begin();
		for (; goalItr != conjGoal->getGoals()->end(); goalItr++) {
			const VAL::goal * aGoal = *goalItr;
			output << "\t\t\t\t" << getGoalString(aGoal) << endl;
		}
		output << "\t\t\t)" << endl;
	} else {
		output << "\t\t\t(and " << endl;
		output << "\t\t\t\t(at start (" << INITIAL_ACTION_COMPLETE_PROPOSITION
				<< "))" << std::endl;
		output << getGoalString(goal);
	}
	return output.str();
}

std::string PDDLDomainFactory::getInitialAction() {
	ostringstream output;
	output << "\t(:action init-action" << endl;
	output << "\t\t:parameters()" << endl << "\t\t:precondition ( )" << endl
			<< "\t\t:effect " << endl << "\t\t\t("
			<< PDDLDomainFactory::INITIAL_ACTION_COMPLETE_PROPOSITION << ")"
			<< endl << "\t)" << endl;
	return output.str();
}

string PDDLDomainFactory::getdeTILedActions(std::list<TIL> tils) {
	//sort list by timestamp
	tils.sort(PDDL::TIL::TILTimestampComparator);
	//list to hold previous TIL predicates
	std::list<PDDL::Proposition> tilActionPreconditions;
	//Look at each TIL one at a time;
	ostringstream output;
	std::list<TIL>::const_iterator tilItr = tils.begin();
	const std::list<TIL>::const_iterator tilItrEnd = tils.end();
	for (; tilItr != tilItrEnd; tilItr++) {
		output << getdeTILedAction(*tilItr, &tilActionPreconditions) << endl;
	}
	return output.str();
}

string PDDLDomainFactory::getdeTILedAction(const TIL & til,
		std::list<PDDL::Proposition> * tilActionPreconditions) {
	//Create special proposition for this TIL
	std::list<string> arguments;
	PDDL::Proposition tilLit(til.getName(), arguments);

	//Determine Parameters
	std::map<const PDDLObject *, std::string> parameterTable =
			PDDL::generateParameterTable(til.getParameters());

	//Generate action string
	ostringstream output;
	output << "\t(:action " << til.getName() << endl;
	output << "\t\t:parameters( ";
	std::map<const PDDLObject *, std::string>::const_iterator paramItr =
			parameterTable.begin();
	for (; paramItr != parameterTable.end(); paramItr++) {
		output << paramItr->second << " - " << paramItr->first->getTypeString()
				<< " ";
	}
	output << ")" << endl;
	output << "\t\t:precondition (and" << endl;
	//Add requirement for initial action
	output << "\t\t\t("
			<< PDDLDomainFactory::INITIAL_ACTION_COMPLETE_PROPOSITION << ")"
			<< std::endl;
	//Add requiredment that TIL hasn't happened (one shot)
	output << "\t\t\t(not (" << til.getName() << "))" << std::endl;
	//Add requirement for past TILs to have been achieved
	if (tilActionPreconditions->size()) {
		std::list<PDDL::Proposition>::const_iterator preItr =
				tilActionPreconditions->begin();
		const std::list<PDDL::Proposition>::const_iterator preItrEnd =
				tilActionPreconditions->end();
		for (; preItr != preItrEnd; preItr++) {
			output << "\t\t\t" << preItr->toParameterisedString(parameterTable)
					<< endl;
		}
	}
	output << "\t\t)" << endl;
	output << "\t\t:effect (and" << endl;
	std::list<PDDL::Proposition>::const_iterator addItr =
			til.getAddEffects().begin();
	const std::list<PDDL::Proposition>::const_iterator addItrEnd =
			til.getAddEffects().end();
	for (; addItr != addItrEnd; addItr++) {
		output << "\t\t\t" << addItr->toParameterisedString(parameterTable)
				<< endl;
	}
	// add special predicate to indicate that the til is complete
	output << "\t\t\t" << tilLit << endl;

	std::list<PDDL::Proposition>::const_iterator delItr =
			til.getDelEffects().begin();
	const std::list<PDDL::Proposition>::const_iterator delItrEnd =
			til.getDelEffects().end();
	for (; delItr != delItrEnd; delItr++) {
		output << "\t\t\t(not " << delItr->toParameterisedString(parameterTable)
				<< ")" << endl;
	}
	output << "\t\t)" << endl << "\t)" << endl;
	//Add the TIL Proposition to the list of preconditions for future TILS
	//This retains precedence ordering of TILs
	tilActionPreconditions->push_back(tilLit);
	return output.str();
}

std::string PDDLDomainFactory::getPendingActions(
		const std::list<PendingAction> & pendingActions) {
	ostringstream output;
	std::list<PendingAction>::const_iterator actItr = pendingActions.begin();
	const std::list<PendingAction>::const_iterator actItrEnd =
			pendingActions.end();
	for (; actItr != actItrEnd; actItr++) {
		output << (*actItr) << endl;
	}
	return output.str();
}

std::string PDDLDomainFactory::getTerminationString() {
	return ")";
}

}
