/*
 * TRH.h
 *
 *  Created on: 26 Jan 2016
 *      Author: tony
 */

#ifndef COLIN_TRH_TRH_H_
#define COLIN_TRH_TRH_H_

#include "PDDLState.h"

using namespace std;

namespace TRH {
class TRH {
private:
	static const char * H_CMD;
	static const string H_VAL_DELIM;
	static TRH * INSTANCE;
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
	double getHeuristic(PDDL::PDDLState state);
	static int STATES_EVALUATED;
	static double TIME_SPENT_IN_HEURISTIC;
	static double TIME_SPENT_IN_PRINTING_TO_FILE;
	static double TIME_SPENT_CONVERTING_PDDL_STATE;
};
}

#endif /* COLIN_TRH_TRH_H_ */
