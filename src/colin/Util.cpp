/*
 * Util.cpp
 *
 *  Created on: 28 Apr 2015
 *      Author: tony
 */

#include "Util.h"
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

void Planner::printState(const MinimalState & theState, double timeStamp) {

	//Print Literal Facts
	const StateFacts & stateFacts = theState.first;
	cout << "There are " << stateFacts.size() << " state facts.\n";
	StateFacts::const_iterator cfItr = stateFacts.begin();
	const StateFacts::const_iterator cfEnd = stateFacts.end();
	for (; cfItr != cfEnd; cfItr++) {
		int stateFact = *cfItr;
		Inst::Literal * literal = RPGBuilder::getLiteral(stateFact);
		if (literal->getStateID() >= 0) {
			printLiteral(literal);
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
	list<RPGBuilder::FakeTILAction> tils = Planner::RPGBuilder::getTILs();
	cout << "There are " << tils.size() << " TILs\n";
	std::list<RPGBuilder::FakeTILAction>::const_iterator tilItr = tils.begin();
	const std::list<RPGBuilder::FakeTILAction>::const_iterator tilItrEnd =
			tils.end();
	for (; tilItr != tilItrEnd; tilItr++) {
		if ((*tilItr).duration < timeStamp) {
			continue;
		}
		cout << "(at " << (*tilItr).duration << " ";
		if ((*tilItr).addEffects.size() > 0) {
			list<Literal*> addEffects = (*tilItr).addEffects;
			std::list<Inst::Literal*>::const_iterator tilAddLitInt =
					addEffects.begin();
			std::list<Inst::Literal*>::const_iterator tilAddLitIntEnd =
					addEffects.end();
			for (; tilAddLitInt != tilAddLitIntEnd; tilAddLitInt++) {
				Inst::Literal * literal = (*tilAddLitInt);
				printLiteral(literal);
			}
		}

		if ((*tilItr).delEffects.size() > 0) {
			cout << "(not ";
			list<Literal*> delEffects = (*tilItr).delEffects;
			std::list<Inst::Literal*>::const_iterator tilDelLitInt =
					delEffects.begin();
			std::list<Inst::Literal*>::const_iterator tilDelLitIntEnd =
					delEffects.end();
			for (; tilDelLitInt != tilDelLitIntEnd; tilDelLitInt++) {
				Inst::Literal * literal = (*tilDelLitInt);
				printLiteral(literal);
			}
			cout << " )\n";
		}
	}
}

void Planner::printLiteral(Literal * literal) {
	VAL::parameter_symbol_list::iterator litIt =
			literal->getProp()->args->begin();
	VAL::parameter_symbol_list::iterator litItEnd =
			literal->getProp()->args->end();
	cout << "(" << literal->getProp()->head->getName() << " ";
	for (; litIt != litItEnd; litIt++) {
		cout << (*litIt)->getName() << " ";
	}
	cout << ")";
}

bool Planner::isSearchNodeValid(SearchQueueItem * searchNode) {
	const static int TOO_MANY_VARIABLES = 10000000;
	if (searchNode->heuristicValue.makespanEstimate
			!= searchNode->heuristicValue.makespanEstimate) {
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

void Planner::printSearchNodeHeuristic(const SearchQueueItem * searchNode) {
	HTrio heuristic = searchNode->heuristicValue;
	cout << "Search Node: (heuristicValue " << heuristic.heuristicValue
			<< ", makespan " << heuristic.makespan << ", makespanEstimate "
			<< heuristic.makespanEstimate << ")\n";
}

