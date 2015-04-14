/*
 * MonteCarloPlanning.cpp
 *
 *  Created on: 09 Apr 2015
 *      Author: tony
 */
#include "MonteCarloPlanning.h";

using namespace MCPlanning;


int MonteCarloPlanning::MonteCarloPlanning(const Planner::MinimalState & state) {
	while (!timeOut) {
		search(state, 0);
	}
	return bestAction(state, 0);
}

int MonteCarloPlanning::search (const Planner::MinimalState & state, int depth) {
	if (isTerminal(state)) return 0;
	if (isLeaf(state)) return evaluate(state);
	ActionSegment action = selectAction(state, depth);
	TransitionResult result = simulateAction(state, action);
	int q = result.getReward() + (gamma * search(result.getNextState(), depth + 1);
	updateValue(state, action, q, depth);
	return q;
}

const TransitionResult & MonteCarloPlanning::simulateAction(const Planner::MinimalState & state, Planner::ActionSegment & action) {
	//Globals::remainingActionsInPlan - might need this to determine available actions
	//Will need something to get TILs

	//Get set of valid actions
	//Determine action selection via UCT
	//Generate TransitionResult and return
}



