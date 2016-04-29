/*
 * TRH.h
 *
 *  Created on: 26 Jan 2016
 *      Author: tony
 */

#ifndef COLIN_TRH_TRH_H_
#define COLIN_TRH_TRH_H_

#include "PDDLState.h"
#include "PDDLStateFactory.h"
#include "../minimalstate.h"
#include "../FFEvent.h"

using namespace std;

namespace TRH {
class TRH {
private:
	static const char * H_CMD;
	static const string H_VAL_DELIM;
	static const string H_STATES_EVAL_DELIM;
	static const string H_PLAN_DELIM;
	static TRH * INSTANCE;
	void writeTempStates(const Planner::MinimalState & state,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, PDDL::PDDLStateFactory pddlFactory);
	//Singleton
	TRH() {
	}
	;
	TRH(TRH const & other) {
	}
	;
	TRH& operator=(TRH const&) {
	}
	;
public:
	static TRH * getInstance();
	double getHeuristic(const Planner::MinimalState & state,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, PDDL::PDDLStateFactory pddlFactory);
	static double TIME_SPENT_IN_HEURISTIC;
	static double TIME_SPENT_IN_PRINTING_TO_FILE;
	static double TIME_SPENT_CONVERTING_PDDL_STATE;
};
}

#endif /* COLIN_TRH_TRH_H_ */
