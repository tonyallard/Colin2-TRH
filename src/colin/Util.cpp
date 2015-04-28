/*
 * Util.cpp
 *
 *  Created on: 28 Apr 2015
 *      Author: tony
 */

#include "Util.h"
#include "instantiation.h"
#include "RPGBuilder.h"

#include <iostream>

using namespace std;

void Planner::printAllLiterals() {
	cout << "There are " << Inst::instantiatedOp::howMany()
			<< " literals. They are:\n";
	for (int j = 0; j < Inst::instantiatedOp::howMany(); j++) {
		Inst::Literal * aliteral = Planner::RPGBuilder::getLiteral(j);
		if (aliteral->getProp() == 0) {
			cout << "check true\n";
			continue;
		}
		cout << "check false\n";
		cout << (j + 1) << ": (" << aliteral->getProp()->head->getName() << " ";
		VAL::parameter_symbol_list::iterator itrArg =
				aliteral->getProp()->args->begin();
		VAL::parameter_symbol_list::iterator itrArgEnd =
				aliteral->getProp()->args->end();

		for (; itrArg != itrArgEnd; itrArg++) {
			cout << (*itrArg)->getName() << "(" << (*itrArg)->type->getName()
					<< ") ";
		}
		cout << ")\n";
	}
}


void Planner::printState(const Planner::MinimalState & theState) {

	//Print Literal Facts
	const Planner::StateFacts & stateFacts = theState.first;
	cout << "There are " << stateFacts.size() << " state facts.\n";
	Planner::StateFacts::const_iterator cfItr = stateFacts.begin();
	const Planner::StateFacts::const_iterator cfEnd = stateFacts.end();
	for (; cfItr != cfEnd; cfItr++) {
		int stateFact = *cfItr;
		Inst::Literal * literal = Planner::RPGBuilder::getLiteral(stateFact);
		if (literal->getStateID() >= 0) {
			VAL::parameter_symbol_list::iterator itrArg =
					literal->getProp()->args->begin();
			VAL::parameter_symbol_list::iterator itrArgEnd =
					literal->getProp()->args->end();
			cout << literal->getProp()->head->getName() << ": ";
			for (; itrArg != itrArgEnd; itrArg++) {
				cout << (*itrArg)->getName() << " ";
			}
			cout << "\n";
		}
	}

	//Print Fluents
	const int pneCount = theState.secondMin.size();
	cout << "There are " << pneCount << " primitive numerical expressions.\n";
	for (int i = 0; i < pneCount; i++) {
		Inst::PNE* pne = Planner::RPGBuilder::getPNE(i);
		cout << "(= " << *pne << " ";
		cout << theState.secondMin[i] << ")\n";
	}
	//Print TILs
	list<Planner::RPGBuilder::FakeTILAction> tils = Planner::RPGBuilder::getTILs();
	cout << "There are " << tils.size() << " TILs\n";
	std::list<Planner::RPGBuilder::FakeTILAction>::const_iterator tilItr = tils.begin();
	const std::list<Planner::RPGBuilder::FakeTILAction>::const_iterator tilItrEnd =
			tils.end();
	for (; tilItr != tilItrEnd; tilItr++) {
		list<Literal*> addEffects = (*tilItr).addEffects;
		std::list<Inst::Literal*>::const_iterator tilLitInt = addEffects.begin();
		std::list<Inst::Literal*>::const_iterator tilLitIntEnd = addEffects.end();
		for (; tilLitInt != tilLitIntEnd; tilLitInt++) {
			Inst::Literal * literal = (*tilLitInt);
			VAL::parameter_symbol_list::iterator itrArg =
					literal->getProp()->args->begin();
			VAL::parameter_symbol_list::iterator itrArgEnd =
					literal->getProp()->args->end();
			cout << literal->getProp()->head->getName() << ": ";
			for (; itrArg != itrArgEnd; itrArg++) {
				cout << (*itrArg)->getName() << " ";
			}
			cout << "\n";
		}
	}
}

bool Planner::isSearchNodeValid(SearchQueueItem * searchNode) {
	const static int TOO_MANY_VARIABLES = 10000000;
	if (searchNode->heuristicValue.makespanEstimate != searchNode->heuristicValue.makespanEstimate) {
		return false; //makespan estimate is nan
	} else if (searchNode->heuristicValue.makespanEstimate < 1.0) {
		return false; // makespan estimate is invalid
	} else if (searchNode->heuristicValue.makespan < 1.0) {
		return false; // makespan is invalid;
	}
	if (searchNode->state() == 0) {
		return false; //state is null
	}
	ExtendedMinimalState * state = searchNode->state();
	if (state->getInnerState().first.size() > TOO_MANY_VARIABLES) {
		return false;
	} else if (state->getInnerState().secondMin.size() > TOO_MANY_VARIABLES) {
		return false;
	}
	return true;
}

//void Planner::printSearchNodeHeuristic(const Planner::SearchQueueItem * searchNode) {
//	FF:HTrio heuristic = searchNode->heuristicValue;
//	cout << "Search Node: (heuristicValue " << heuristic.heuristicValue <<", makespan " << heuristic.makespan << ", makespanEstimate " << heuristic.makespanEstimate << ")\n";
//}

