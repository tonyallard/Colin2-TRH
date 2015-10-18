/*
 * PDDLMMCRDomainFactory.cpp
 *
 *  Created on: 18 Jul 2015
 *      Author: tony
 */

#include <sstream>

#include "MMCRDomainFactory.h"
#include "TIL.h"
using namespace std;
namespace PDDL {

const std::string MMCRDomainFactory::TIL_ACHIEVED_PROPOSITION = "til-achieved";

std::string MMCRDomainFactory::getMMCRDomain(
		const std::list<PendingAction> & pendingActions) {
	ostringstream output;
	output << getHeader(false) << getTypes(false) << getPredicates(false)
			<< getFunctions() << getLoadAction() << getUnloadAction()
			<< getMoveAction() << getPendingActions(pendingActions)
			<< getTerminationString();
	return output.str();
}

std::string MMCRDomainFactory::getDeTILedMMCRDomain(std::list<TIL> tils,
		const std::list<PendingAction> & pendingActions) {
	ostringstream output;
	bool hasTils = tils.size();
	output << getHeader(hasTils) << getTypes(hasTils) << getPredicates(hasTils)
			<< getFunctions() << getLoadAction() << getUnloadAction()
			<< getMoveAction() << getPendingActions(pendingActions);
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
			<< "\t(:requirements :typing :equality :fluents :action-costs :durative-actions :duration-inequalities";
	if (!deTILed) {
		output << " :timed-initial-literals";
	}
	output << ")\n";
	return output.str();
}

std::string MMCRDomainFactory::getTypes(bool deTILed) {
	ostringstream output;
	output << "\t(:types\n";
	output << "\t\tCONTAINER CARGO - object\n";
	output << "\t\tVEHICLE LOCATION - CONTAINER\n";
	if (deTILed) {
		output << "\t\tTIL - object\n";
	}
	output << "\t)\n";
	return output.str();
}

std::string MMCRDomainFactory::getPredicates(bool deTILed) {
	ostringstream output;
	output << "\t(:predicates\n";
	output << "\t\t(at ?x - (either VEHICLE CARGO) ?y - LOCATION)\n";
	output << "\t\t(in ?x - CARGO ?y - VEHICLE)\n";
	output << "\t\t(ready-loading ?x - VEHICLE)\n";
	output << "\t\t(available ?x - (either VEHICLE CARGO))\n";
	if (deTILed) {
		output << "\t\t(" << MMCRDomainFactory::TIL_ACHIEVED_PROPOSITION
				<< " ?x - TIL)\n";
	}
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
	arguments.push_back(til.getName());
	PDDL::Proposition tilLit(MMCRDomainFactory::TIL_ACHIEVED_PROPOSITION, arguments);

	//Generate action string
	ostringstream output;
	output << "\t(:action " << til.getName() << "\n";
	output << "\t\t:parameters()\n";
	output << "\t\t:precondition (";
	if (tilActionPreconditions->size()) {
		if (tilActionPreconditions->size() > 1) {
			output << "\t(and";
		}
		output << "\n";
		std::list<PDDL::Proposition>::const_iterator preItr =
				tilActionPreconditions->begin();
		const std::list<PDDL::Proposition>::const_iterator preItrEnd =
				tilActionPreconditions->end();
		for (; preItr != preItrEnd; preItr++) {
			output << "\t\t\t" << *preItr << "\n";
		}
		if (tilActionPreconditions->size() > 1) {
			output << ")";
		}
	}
	output << ")\n";
	output << "\t\t:effect (";
	//We add an extra one here for the TIL Proposition itself
	if (til.getAddEffects().size() + til.getDelEffects().size() + 1 > 1) {
		output << "\t\t(and\n";
	}
	std::list<PDDL::Proposition>::const_iterator addItr =
			til.getAddEffects().begin();
	const std::list<PDDL::Proposition>::const_iterator addItrEnd =
			til.getAddEffects().end();
	for (; addItr != addItrEnd; addItr++) {
		output << "\t\t\t" << *addItr << "\n";
	}
	// add special predicate to indicate that the til is complete
	output << "\t\t\t" << tilLit << "\n";

	std::list<PDDL::Proposition>::const_iterator delItr =
			til.getDelEffects().begin();
	const std::list<PDDL::Proposition>::const_iterator delItrEnd =
			til.getDelEffects().end();
	for (; delItr != delItrEnd; delItr++) {
		output << "\t\t\t(not " << *delItr << ")\n";
	}
	//We add an extra one here for the TIL Proposition itself
	if (til.getAddEffects().size() + til.getDelEffects().size() + 1 > 1) {
		output << ")";
	}
	output << "\t))\n";
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
