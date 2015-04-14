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

}

