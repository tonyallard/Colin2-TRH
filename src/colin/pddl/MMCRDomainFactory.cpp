/*
 * PDDLMMCRDomainFactory.cpp
 *
 *  Created on: 18 Jul 2015
 *      Author: tony
 */

#include <sstream>

#include "MMCRDomainFactory.h"
#include "TIL.h"
#include "PDDLUtils.h"

using namespace std;
namespace PDDL {

const std::string MMCRDomainFactory::TIL_ACHIEVED_PROPOSITION = "til-achieved";
const std::string MMCRDomainFactory::REQUIRED_PROPOSITION = "required";
const std::string MMCRDomainFactory::INITIAL_ACTION_COMPLETE_PROPOSITION =
		"initial-action-complete";

std::string MMCRDomainFactory::getMMCRDomain(
		const std::list<PendingAction> & pendingActions) {
	ostringstream output;
	output << getHeader(false) << getTypes() << getPredicates()
			<< getFunctions() << getLoadAction() << getUnloadAction()
			<< getMoveAction() << getPendingActions(pendingActions)
			<< getTerminationString();
	return output.str();
}

std::string MMCRDomainFactory::getDeTILedMMCRDomain(const std::list<TIL> & tils,
		const std::list<PendingAction> & pendingActions) {
	ostringstream output;
	bool hasTils = tils.size();
	output << getHeader(hasTils) << getTypes()
			<< getPredicates(pendingActions, tils) << getFunctions()
			<< getLoadAction() << getUnloadAction() << getMoveAction()
			<< getInitialAction() << getPendingActions(pendingActions);
	if (hasTils) {
		output << getdeTILedActions(tils);
	}
	output << getTerminationString();
	return output.str();
}

std::string MMCRDomainFactory::getHeader(bool deTILed) {
	ostringstream output;
	output << "(define (domain multi-modal-cargo-routing)\n";
	output
			<< "\t(:requirements :typing :equality :fluents :action-costs :durative-actions :duration-inequalities :negative-preconditions";
	if (!deTILed) {
		output << " :timed-initial-literals";
	}
	output << ")\n";
	return output.str();
}

std::string MMCRDomainFactory::getTypes() {
	ostringstream output;
	output << "\t(:types\n";
	output << "\t\tCONTAINER CARGO - object\n";
	output << "\t\tVEHICLE LOCATION - CONTAINER\n";
	output << "\t)\n";
	return output.str();
}

std::string MMCRDomainFactory::getPredicates(
		const std::list<PendingAction> & pendingActions /*=empty list*/,
		const std::list<TIL> & tils /*=empty list*/) {
	ostringstream output;
	output << "\t(:predicates\n";
	output << "\t\t(at ?x - (either VEHICLE CARGO) ?y - LOCATION)\n";
	output << "\t\t(in ?x - CARGO ?y - VEHICLE)\n";
	output << "\t\t(ready-loading ?x - VEHICLE)\n";
	output << "\t\t(available ?x - (either VEHICLE CARGO))\n";
	std::list<TIL>::const_iterator tilItr = tils.begin();
	for (; tilItr != tils.end(); tilItr++) {
		output << "\t\t(" << tilItr->getName() << ")\n";
	}
	if (pendingActions.size()) {
		std::list<PendingAction>::const_iterator pendActItr =
				pendingActions.begin();
		for (; pendActItr != pendingActions.end(); pendActItr++) {
			output << "\t\t(" << MMCRDomainFactory::REQUIRED_PROPOSITION << "-"
					<< pendActItr->getName() << " ?x - object)\n";
		}
	}
	output << "\t\t(" << MMCRDomainFactory::INITIAL_ACTION_COMPLETE_PROPOSITION
			<< ")\n";
	output << "\t)\n";
	return output.str();
}

std::string MMCRDomainFactory::getFunctions() {
	ostringstream output;
	output << "\t(:functions\n";
	output << "\t\t(remaining-capacity ?x - CONTAINER)\n";
	output << "\t\t(travel-time ?x - VEHICLE ?y ?z - LOCATION)\n";
	output << "\t\t(size ?x - CARGO)\n";
	output << "\t\t(load-time ?x - VEHICLE ?y - Location)\n";
	output << "\t\t(unload-time ?x - VEHICLE ?y - Location)\n";
	output << "\t\t(cost ?x - VEHICLE)\n";
	output << "\t\t(total-cost)\n";
	output << "\t)\n";
	return output.str();
}

std::string MMCRDomainFactory::getLoadAction() {
	ostringstream output;
	output << "\t(:durative-action load\n";
	output << "\t\t:parameters (?x - VEHICLE ?y - CARGO ?z - LOCATION)\n";
	output << "\t\t:duration (= ?duration (* (load-time ?x ?z) (size ?y)))\n";
	output << "\t\t:condition\t(and\n";
	output << "\t\t\t(at start ("
			<< MMCRDomainFactory::INITIAL_ACTION_COMPLETE_PROPOSITION << "))\n";
	output << "\t\t\t(over all (at ?x ?z))\n";
	output << "\t\t\t(at start (ready-loading ?x))\n";
	output << "\t\t\t(at start (at ?y ?z))\n";
	output << "\t\t\t(at start (<= (size ?y) (remaining-capacity ?x)))\n";
	output << "\t\t\t(at start (available ?y))\n";
	output << "\t\t\t(over all (available ?x))\n";
	output << "\t\t\t(over all (available ?y)))\n";
	output << "\t\t:effect\t(and\n";
	output << "\t\t\t(at start (not (at ?y ?z)))\n";
	output << "\t\t\t(at start (decrease (remaining-capacity ?x) (size ?y)))\n";
	output << "\t\t\t(at start (not (ready-loading ?x)))\n";
	output << "\t\t\t(at end (increase (remaining-capacity ?z) (size ?y)))\n";
	output << "\t\t\t(at end (in ?y ?x))\n";
	output << "\t\t\t(at end (ready-loading ?x))\n";
	output
			<< "\t\t\t(at end (increase (total-cost) (* ?duration (cost ?x))))))\n";
	return output.str();
}

std::string MMCRDomainFactory::getUnloadAction() {
	ostringstream output;
	output << "\t(:durative-action unload\n";
	output << "\t\t:parameters (?x - VEHICLE ?y - CARGO ?z - LOCATION)\n";
	output << "\t\t:duration (= ?duration (* (unload-time ?x ?z) (size ?y)))\n";
	output << "\t\t:condition\t(and\n";
	output << "\t\t\t(at start ("
			<< MMCRDomainFactory::INITIAL_ACTION_COMPLETE_PROPOSITION << "))\n";
	output << "\t\t\t(over all (at ?x ?z))\n";
	output << "\t\t\t(at start (ready-loading ?x))\n";
	output << "\t\t\t(at start (in ?y ?x))\n";
	output << "\t\t\t(at start (<= (size ?y) (remaining-capacity ?z)))\n";
	output << "\t\t\t(over all (available ?x))\n";
	output << "\t\t\t(over all (available ?y)))\n";
	output << "\t\t:effect\t(and\n";
	output << "\t\t\t(at start (not (in ?y ?x)))\n";
	output << "\t\t\t(at start (decrease (remaining-capacity ?z) (size ?y)))\n";
	output << "\t\t\t(at start (not (ready-loading ?x)))\n";
	output << "\t\t\t(at end (increase (remaining-capacity ?x) (size ?y)))\n";
	output << "\t\t\t(at end (at ?y ?z))\n";
	output << "\t\t\t(at end (ready-loading ?x))\n";
	output
			<< "\t\t\t(at end (increase (total-cost) (* ?duration (cost ?x))))))\n";
	return output.str();
}

std::string MMCRDomainFactory::getMoveAction() {
	ostringstream output;
	output << "\t(:durative-action move\n";
	output << "\t\t:parameters (?x - VEHICLE ?y ?z - LOCATION)\n";
	output << "\t\t:duration (= ?duration (travel-time ?x ?y ?z))\n";
	output << "\t\t:condition\t(and\n";
	output << "\t\t\t(at start ("
			<< MMCRDomainFactory::INITIAL_ACTION_COMPLETE_PROPOSITION << "))\n";
	output << "\t\t\t(at start (at ?x ?y))\n";
	output << "\t\t\t(at start (>= (travel-time ?x ?y ?z) 0))\n";
	output << "\t\t\t(at start (not (= ?y ?z)))\n";
	output << "\t\t\t(over all (available ?x)))\n";
	output << "\t\t:effect\t(and\n";
	output << "\t\t\t(at start (not (at ?x ?y)))\n";
	output << "\t\t\t(at end (at ?x ?z))\n";
	output
			<< "\t\t\t(at end (increase (total-cost) (* ?duration (cost ?x))))))\n";
	return output.str();
}

std::string MMCRDomainFactory::getInitialAction() {
	ostringstream output;
	output << "\t(:action init-action" << endl;
	output << "\t\t:parameters()" << endl << "\t\t:precondition ( )" << endl
			<< "\t\t:effect " << endl << "\t\t\t("
			<< MMCRDomainFactory::INITIAL_ACTION_COMPLETE_PROPOSITION << ")"
			<< endl << "\t)" << endl;
	return output.str();
}

string MMCRDomainFactory::getdeTILedActions(std::list<TIL> tils) {
	//sort list by timestamp
	tils.sort(PDDL::TIL::TILTimestampComparator);
	//list to hold previous TIL predicates
	std::list<PDDL::Proposition> tilActionPreconditions;
	//Look at each TIL one at a time;
	ostringstream output;
	std::list<TIL>::const_iterator tilItr = tils.begin();
	const std::list<TIL>::const_iterator tilItrEnd = tils.end();
	for (; tilItr != tilItrEnd; tilItr++) {
		output << getdeTILedAction(*tilItr, &tilActionPreconditions) << "\n";
	}
	return output.str();
}

string MMCRDomainFactory::getdeTILedAction(const TIL & til,
		std::list<PDDL::Proposition> * tilActionPreconditions) {
	//Create special proposition for this TIL
	std::list<string> arguments;
	PDDL::Proposition tilLit(til.getName(), arguments);

	//Determine Parameters
	std::map<const PDDLObject *, std::string> parameterTable =
			PDDL::generateParameterTable(til.getParameters());

	//Generate action string
	ostringstream output;
	output << "\t(:action " << til.getName() << "\n";
	output << "\t\t:parameters( ";
	std::map<const PDDLObject *, std::string>::const_iterator paramItr =
			parameterTable.begin();
	for (; paramItr != parameterTable.end(); paramItr++) {
		output << paramItr->second << " - " << paramItr->first->getType()
				<< " ";
	}
	output << ")\n";
	output << "\t\t:precondition (and\n";
	//Add requirement for initial action
	output << "\t\t\t("
			<< MMCRDomainFactory::INITIAL_ACTION_COMPLETE_PROPOSITION << ")"
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
					<< "\n";
		}
	}
	output << "\t\t)\n";
	output << "\t\t:effect (and\n";
	std::list<PDDL::Proposition>::const_iterator addItr =
			til.getAddEffects().begin();
	const std::list<PDDL::Proposition>::const_iterator addItrEnd =
			til.getAddEffects().end();
	for (; addItr != addItrEnd; addItr++) {
		output << "\t\t\t" << addItr->toParameterisedString(parameterTable)
				<< "\n";
	}
	// add special predicate to indicate that the til is complete
	output << "\t\t\t" << tilLit << "\n";

	std::list<PDDL::Proposition>::const_iterator delItr =
			til.getDelEffects().begin();
	const std::list<PDDL::Proposition>::const_iterator delItrEnd =
			til.getDelEffects().end();
	for (; delItr != delItrEnd; delItr++) {
		output << "\t\t\t(not " << delItr->toParameterisedString(parameterTable)
				<< ")\n";
	}
	output << "\t\t)\n\t)";
	//Add the TIL Proposition to the list of preconditions for future TILS
	//This retains precedence ordering of TILs
	tilActionPreconditions->push_back(tilLit);
	return output.str();
}

std::string MMCRDomainFactory::getPendingActions(
		const std::list<PendingAction> & pendingActions) {
	ostringstream output;
	std::list<PendingAction>::const_iterator actItr = pendingActions.begin();
	const std::list<PendingAction>::const_iterator actItrEnd =
			pendingActions.end();
	for (; actItr != actItrEnd; actItr++) {
		output << (*actItr) << "\n";
	}
	return output.str();
}

std::string MMCRDomainFactory::getTerminationString() {
	return ")";
}

}
