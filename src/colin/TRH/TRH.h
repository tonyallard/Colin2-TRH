/*
 * TRH.h
 *
 *  Created on: 26 Jan 2016
 *      Author: tony
 */

#ifndef COLIN_TRH_TRH_H_
#define COLIN_TRH_TRH_H_

#include <list>

#include "PDDLState.h"
#include "PDDLStateFactory.h"
#include "../minimalstate.h"
#include "../FFEvent.h"
#include "../util/Util.h"

using namespace std;

namespace TRH {
class TRH {
private:
	static const char * H_CMD;
	static const string H_VAL_DELIM;
	static const string RELAXED_PLAN_SIZE_DELIM;
	static const string H_STATES_EVAL_DELIM;
	static const string H_PLAN_DELIM;
	static const string TEMP_STATE_PATH;
	static TRH * INSTANCE;

	static int generateNewInstanceID();
	
	/*Used to ensure unique state files per instance*/
	const int TRH_INSTANCE_ID;

	//Singleton
	TRH(int trhInstanceID) : TRH_INSTANCE_ID(trhInstanceID) {
	};
	TRH(TRH const & other) : TRH_INSTANCE_ID(generateNewInstanceID()){
	}
	;
	TRH& operator=(TRH const&) {
	}
	;

	string buildCommand();
	string writeTempState(const Planner::MinimalState & state,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, 
		PDDL::PDDLStateFactory pddlFactory);
	void writeBadState(const Planner::MinimalState & state,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, 
		PDDL::PDDLStateFactory pddlFactory, int stateNum);
	void writeStateToFile(const Planner::MinimalState & state,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, 
		PDDL::PDDLStateFactory pddlFactory, string fileName);
	void removeTempState(string fileName);
	list<Util::triple<double, string, double> > getRelaxedPlan(string plan);

public:
	static TRH * getInstance();
	pair<double, int> getHeuristic(const Planner::MinimalState & state,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, PDDL::PDDLStateFactory pddlFactory);
	static double TIME_SPENT_IN_HEURISTIC;
	static double TIME_SPENT_IN_PRINTING_TO_FILE;
	static double TIME_SPENT_CONVERTING_PDDL_STATE;
};
}

#endif /* COLIN_TRH_TRH_H_ */
