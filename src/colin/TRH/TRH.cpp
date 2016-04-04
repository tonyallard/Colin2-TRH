#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstdio>
#include <memory>

#include "TRH.h"
#include "../FFSolver.h"

namespace TRH {

TRH * TRH::INSTANCE = NULL;
const char * TRH::H_CMD = "./lib/colin-clp tempdomain.pddl temp.pddl";
const string TRH::H_VAL_DELIM = "State Heuristic Value is: ";
const string TRH::H_STATES_EVAL_DELIM = "; States evaluated: ";
const string TRH::H_PLAN_DELIM = "(init-action)  [0.001]";

double TRH::TIME_SPENT_IN_HEURISTIC = 0.0;
double TRH::TIME_SPENT_IN_PRINTING_TO_FILE = 0.0;
double TRH::TIME_SPENT_CONVERTING_PDDL_STATE = 0.0;

TRH * TRH::getInstance() {
	if (!INSTANCE) {
		INSTANCE = new TRH();
	}
	return INSTANCE;
}

double TRH::getHeuristic(PDDL::PDDLState state) {

    Planner::FF::STATES_EVALUATED++;
	clock_t begin_time = clock();
	string filePath = "";
	string fileName = "temp";
	state.writeDeTILedStateToFile(filePath, fileName);
	state.writeDeTILedDomainToFile(filePath, fileName);
	TRH::TRH::TIME_SPENT_IN_PRINTING_TO_FILE += float( clock () - begin_time ) /  CLOCKS_PER_SEC;
	begin_time = clock();
	std::shared_ptr<FILE> pipe(popen(H_CMD, "r"), pclose);
	TRH::TRH::TIME_SPENT_IN_HEURISTIC += float( clock () - begin_time ) /  CLOCKS_PER_SEC;
	if (!pipe)
		return 1e5;
	char buffer[128];
	std::string result = "";
	while (!feof(pipe.get())) {
		if (fgets(buffer, 128, pipe.get()) != NULL)
			result += buffer;
	}
	int pos = result.find(H_STATES_EVAL_DELIM);
	if (pos != -1) {
		int posEnd = result.find("\n", pos);
		string statesEvalStr = result.substr(pos + H_STATES_EVAL_DELIM.size(), posEnd-(pos + H_STATES_EVAL_DELIM.size()));
		int statesEval = stoi(statesEvalStr);
		printf("Heuristic States Eval: %s\n", statesEvalStr.c_str());
		Planner::FF::STATES_EVALUATED_IN_HEURISTIC += statesEval;
	}
	pos = result.find(H_VAL_DELIM);
	if (pos == -1) {
		cerr << "Problem was unsolvable - therefore max heuristic" << endl;
		return 1e5;
	}
	string h_val_str = result.substr(pos + H_VAL_DELIM.size());
	printf("%s\n", h_val_str.c_str());
	double hval = stod(h_val_str);
	int planPos = result.find(H_PLAN_DELIM);
	if ((planPos != -1) && (hval == 0.0)) {
		string plan = result.substr(planPos + H_PLAN_DELIM.size(), pos);
		cout << plan << endl;
	}
	return hval;
}
}
