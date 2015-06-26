/*
 * Util.cpp
 *
 *  Created on: 28 Apr 2015
 *      Author: tony
 */

#include "Util.h"
#include "RPGBuilder.h"

#include <iostream>
#include <sstream>

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
	cout << getStateLiteralsString(theState);
	//Print Fluents
	cout << getStateFluentString(theState);
	//Print TILs
	cout << getStateTILString(theState, timeStamp);
}

string Planner::getLiteralString(Literal * literal) {
	ostringstream output;
	literal->write(output);
	return output.str();
}

string Planner::getStateLiteralsString(const MinimalState & theState) {

	string output;
	//Cycle through State Literal Facts
	const StateFacts & stateFacts = theState.first;
	StateFacts::const_iterator cfItr = stateFacts.begin();
	const StateFacts::const_iterator cfEnd = stateFacts.end();
	for (; cfItr != cfEnd; cfItr++) {
		int stateFact = *cfItr;
		Inst::Literal * literal = RPGBuilder::getLiteral(stateFact);
		output += getLiteralString(literal);
		output += "\n";
	}

	//Look for static literals
	std::vector<pair<bool, bool> > staticLiterals =
			RPGBuilder::getStaticLiterals();
	std::vector<pair<bool, bool> >::const_iterator slItr =
			staticLiterals.begin();
	const std::vector<pair<bool, bool> >::const_iterator slItrEnd =
			staticLiterals.end();
	int i = 0;
	for (; slItr != slItrEnd; slItr++) {
		Inst::Literal * literal = RPGBuilder::getLiteral(i++);
		if (slItr->first && slItr->second) {
			output += getLiteralString(literal) + "\n";
		}
	}

	//Cycle through Facts held up by executing actions (these are effects coming into play)
	ostringstream action;
	std::map<int, std::set<int> >::const_iterator saItr =
			theState.startedActions.begin();
	const std::map<int, std::set<int> >::const_iterator saItrEnd =
			theState.startedActions.end();

	for (; saItr != saItrEnd; saItr++) {
		// Literals Added by Action
		std::list<Literal*> adds =
				RPGBuilder::getEndPropositionAdds()[saItr->first];
		std::list<Literal*>::const_iterator addItr = adds.begin();
		const std::list<Literal*>::const_iterator addItrEnd = adds.end();
		if (addItr != addItrEnd) {
			action << "; Literals added by started action " << saItr->first
					<< endl;
		}
		for (; addItr != addItrEnd; addItr++) {
			action << Planner::getLiteralString(*addItr) << endl;
		}

		// Literals Delted by Action
		std::list<Literal*> deletes =
				RPGBuilder::getEndPropositionDeletes()[saItr->first];
		std::list<Literal*>::const_iterator delItr = deletes.begin();
		const std::list<Literal*>::const_iterator delItrEnd = deletes.end();
		if (delItr != delItrEnd) {
			action << "; Literals removed by started action " << saItr->first
					<< endl;
		}
		for (; delItr != delItrEnd; delItr++) {
			action << Planner::getLiteralString(*delItr) << endl;
		}
		action << "; end started action " << saItr->first << endl;
	}
	output += action.str();
	return output;
}

string Planner::getStateFluentString(const MinimalState & theState) {
	ostringstream output;
	//Cycle through PNEs
	cout
			<< (RPGBuilder::getPNECount() == theState.secondMin.size() ?
					"same" : "diff") << endl;

	const int pneCount = theState.secondMin.size();
	for (int i = 0; i < pneCount; i++) {
		Inst::PNE* pne = Planner::RPGBuilder::getPNE(i);
		output << "(= " << *pne << " ";
		output << theState.secondMin[i] << ")\n";
	}

	//Get PNE differences that are coming from actions
	ostringstream action;
	std::map<int, std::set<int> >::const_iterator saItr =
			theState.startedActions.begin();
	const std::map<int, std::set<int> >::const_iterator saItrEnd =
			theState.startedActions.end();
	for (; saItr != saItrEnd; saItr++) {
		std::list<int> actionEffPNE =
				RPGBuilder::getEndEffNumerics()[saItr->first];
		std::list<int>::const_iterator effItr = actionEffPNE.begin();
		const std::list<int>::const_iterator effItrEnd = actionEffPNE.end();
//		cout << "Action: " << saItr->first << endl;
//		cout << "Number of Action PNEs: " << actionEffPNE.size() << endl;
//		Inst::PNE* pne = Planner::RPGBuilder::getPNE(5);
//		cout << "PNE: " << *pne << theState.secondMin[6] << endl;
		if (effItr != effItrEnd) {
			action << "; Fluents added by action " << saItr->first << endl;
			for (; effItr != effItrEnd; effItr++) {
//			cout << "int stored: " << *effItr << endl;
//			if ((*effItr) > RPGBuilder::getPNECount()) {
//				cerr << "This could be invalid" << endl;
//				cerr << getStateLiteralsString(theState) << endl;
//			}
				//Get the numeric effect information
				RPGBuilder::RPGNumericEffect & effect =
						RPGBuilder::getNumericEff()[*effItr];
				// Get action duration
				double minDur = RPGBuilder::getOpMinDuration(saItr->first, 1);
				double maxDur = RPGBuilder::getOpMaxDuration(saItr->first, 1);
				//Apply the numeric effect to get new fluent values
				pair<double, double> result = effect.applyEffectMinMax(
						theState.secondMin, theState.secondMax, minDur, maxDur);
				//Determine the PNE that the values belong
				Inst::PNE* pne = Planner::RPGBuilder::getPNE(
						effect.fluentIndex);
				action << "(= " << *pne << " " << result.first << ")" << endl;
			}
			action << "; end Fluents Added by " << saItr->first << endl;
			action << endl;
		}
	}
	output << action.str();
	return output.str();
}

string Planner::getMetricString() {
	ostringstream output;
	//Put together metric
	Planner::RPGBuilder::Metric * metric = Planner::RPGBuilder::getMetric();
	std::list<double>::const_iterator weightItr = metric->weights.begin();
	const std::list<double>::const_iterator weightItrEnd =
			metric->weights.end();
	std::list<int>::const_iterator variablesItr = metric->variables.begin();
	const std::list<int>::const_iterator variablesItrEnd =
			metric->variables.end();
	for (; weightItr != weightItrEnd, variablesItr != variablesItrEnd;
			weightItr++, variablesItr++) {
		double weight = *weightItr;
		int variable = *variablesItr;
		output << variable << " " << weight;
	}
	output << "\n";
	return output.str();
}

string Planner::getStateTILString(const MinimalState & theState,
		double timeStamp) {
	ostringstream output;
	//Cycle thourgh TILs
	list<RPGBuilder::FakeTILAction> tils = Planner::RPGBuilder::getTILs();
	std::list<RPGBuilder::FakeTILAction>::const_iterator tilItr = tils.begin();
	const std::list<RPGBuilder::FakeTILAction>::const_iterator tilItrEnd =
			tils.end();
	for (; tilItr != tilItrEnd; tilItr++) {
		if ((*tilItr).duration < timeStamp) {
			continue;
		}
		output << "(at " << (*tilItr).duration << " ";
		if ((*tilItr).addEffects.size() > 0) {
			list<Literal*> addEffects = (*tilItr).addEffects;
			std::list<Inst::Literal*>::const_iterator tilAddLitInt =
					addEffects.begin();
			std::list<Inst::Literal*>::const_iterator tilAddLitIntEnd =
					addEffects.end();
			for (; tilAddLitInt != tilAddLitIntEnd; tilAddLitInt++) {
				Inst::Literal * literal = (*tilAddLitInt);
				output << getLiteralString(literal);
			}
		}

		if ((*tilItr).delEffects.size() > 0) {
			output << "(not ";
			list<Literal*> delEffects = (*tilItr).delEffects;
			std::list<Inst::Literal*>::const_iterator tilDelLitInt =
					delEffects.begin();
			std::list<Inst::Literal*>::const_iterator tilDelLitIntEnd =
					delEffects.end();
			for (; tilDelLitInt != tilDelLitIntEnd; tilDelLitInt++) {
				Inst::Literal * literal = (*tilDelLitInt);
				output << getLiteralString(literal);
			}
			output << " )";
		}
		output << ")\n";
	}
	return output.str();
}

bool Planner::isSearchNodeValid(SearchQueueItem & searchNode) {
	if (searchNode.state() == 0) {
		return false; //state is null
	}
	ExtendedMinimalState * state = searchNode.state();
	if (state->timeStamp < 0) {
		return false;
	}
	if ((searchNode.heuristicValue.qbreak > 0.0)
			&& (searchNode.heuristicValue.qbreak
					< Planner::ERRONEOUS_HEURISTIC_MIN)) {
		return false;
	}
	if (state->getInnerStatePtr() == 0) {
		return false;
	}
	if (searchNode.heuristicValue.makespan
			!= searchNode.heuristicValue.makespan) {
		return false; //makespan is nan
	}
	if (searchNode.state()->timeStamp != searchNode.state()->timeStamp) {
		return false; //timestamp is nan
	}

	if (state->getInnerStatePtr()->actionsExecuting > TOO_MANY_VARIABLES) {
		return false;
	}

	return true;

//	if (searchNode.heuristicValue.makespanEstimate
//			!= searchNode.heuristicValue.makespanEstimate) {
//		return false; //makespan estimate is nan
//	} else if (searchNode.heuristicValue.makespanEstimate < 1.0) {
//		return false; // makespan estimate is invalid
////	} else if (searchNode->heuristicValue.makespan < 1.0) {
//		return false; // makespan is invalid;
//	}
//	if (searchNode.state() == 0) {
//		return false; //state is null
//	}
//	ExtendedMinimalState * state = searchNode.state();
//	if (state->getInnerState().first.size() > Planner::TOO_MANY_VARIABLES) {
//		return false;
//	} else if (state->getInnerState().secondMin.size()
//			> Planner::TOO_MANY_VARIABLES) {
//		return false;
//	}
//	return true;
}

void Planner::printErrorState(SearchQueueItem & searchNode, int stateNum) {
	cout << "Error state found at number " << stateNum << "\n";
	printSearchNodeHeuristic(searchNode);
	if (searchNode.state() != 0) {
		ExtendedMinimalState * state = searchNode.state();
		double timeStamp = searchNode.state()->timeStamp;
		cout << "Timestamp " << timeStamp << "\n";
		if (state->getInnerStatePtr() != 0) {
			if ((searchNode.heuristicValue.qbreak < 0.0)
					|| (searchNode.heuristicValue.qbreak
							> Planner::ERRONEOUS_HEURISTIC_MIN)) {
				cout << "Makespan "
						<< searchNode.state()->getInnerStatePtr()->planLength
						<< "\n";
				cout << "Actions Executing "
						<< searchNode.state()->getInnerStatePtr()->actionsExecuting
						<< "\n";
				cout << "Number of Literals: "
						<< state->getInnerState().first.size() << "\n";
				cout << "Number of Fluents: "
						<< state->getInnerState().secondMin.size() << "\n";
			}
		}
	}

}

void Planner::printSearchNodeHeuristic(const SearchQueueItem & searchNode) {
	HTrio heuristic = searchNode.heuristicValue;
	cout << "Search Node: (heuristicValue " << heuristic.heuristicValue
			<< ", makespan " << heuristic.makespan << ", makespanEstimate "
			<< heuristic.makespanEstimate << ")\n";
}

