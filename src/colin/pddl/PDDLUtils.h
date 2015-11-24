/*
 * PDDLUtils.h
 *
 *  Created on: 25 Aug 2015
 *      Author: tony
 */

#ifndef COLIN_PDDLUTILS_H_
#define COLIN_PDDLUTILS_H_

#include <list>
#include <set>
#include <string>
#include <map>

#include "PDDLObject.h"
#include "Proposition.h"
#include "Literal.h"
#include "PNE.h"
#include "TIL.h"
#include "PendingProposition.h"
#include "PendingPNE.h"

#include "../instantiation.h"
#include "ptree.h"
#include "../FakeTILAction.h"
#include "../FFEvent.h"

namespace PDDL {

//Literal and PNE Helper Functions
std::set<PDDLObject> & extractParameters(Inst::Literal * literal,
		set<PDDLObject> & parameters);
std::set<PDDLObject> & extractParameters(Inst::PNE * pne,
		set<PDDLObject> & parameters);
set<PDDLObject> & extractParameters(
		const VAL::parameter_symbol_list * parameter_symbol_list,
		set<PDDLObject> & parameters);
std::map<const PDDLObject *, std::string> generateParameterTable(
		const std::set<PDDLObject> & parameters);

//Action Helper Functions
std::list<PDDL::Literal> getActionConditions(int actionID,
		VAL::time_spec timeQualifier);
std::list<PDDL::Proposition> getActionEffects(int actionID,
		VAL::time_spec timeQualifier, bool positive);
std::string getActionName(int actionNum);
std::map<std::string, std::string> getActionParameters(int actionNum);
bool supported(const PDDL::Proposition * proposition,
		std::list<PDDL::Proposition> * propositions);
bool isEqual(const Planner::FFEvent * one, const Planner::FFEvent * two);

//TIL Helper Functions
double extractTILTimeStamp(const Planner::FFEvent * tilEvent);

//Basic conversion functions
std::list<PDDL::Proposition> getPropositions(
		std::list<Inst::Literal*> * literals);
PDDL::Proposition getProposition(const Inst::Literal * aLiteral);
PDDL::Literal getLiteral(const Inst::Literal * aLiteral, bool positive);
std::list<PDDL::Literal> getLiterals(std::list<Inst::Literal*> * literals,
		bool positive);
PDDL::PNE getPNE(const Inst::PNE * aPNE, double value);
PDDL::TIL getTIL(Planner::FakeTILAction aTIL, double aTimestamp);
PDDL::PendingProposition getPendingProposition(const Inst::Literal * aLiteral,
		std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions,
		double timestamp, bool isPositive);
PDDL::PendingPNE getPendingPNE(vector<double> minValue, vector<double> maxValue,
		int numeric,
		std::list<std::pair<PDDL::Proposition, std::pair<VAL::time_spec, bool> > > conditions,
		double minDur, double maxDur);

//Plan Helper Functions
std::list<std::string> getPlanPrefix(std::list<Planner::FFEvent>& plan);
bool isAfter(const Planner::FFEvent * event, const Planner::FFEvent * after,
		std::list<Planner::FFEvent> * plan);
bool isBefore(const Planner::FFEvent * event, const Planner::FFEvent * before,
		std::list<Planner::FFEvent> * plan);
std::list<const Planner::FFEvent *> getTILActions(
		std::list<Planner::FFEvent> * plan);
}

#endif /* COLIN_PDDLUTILS_H_ */
