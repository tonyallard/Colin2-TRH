#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstdio>
#include <memory>
#include <sstream>

#include "TRH.h"
#include "PDDLDomain.h"
#include "PDDLState.h"
#include "PDDLDomainFactory.h"
#include "PendingAction.h"
#include "Proposition.h"
#include "PDDLObject.h"
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

double TRH::getHeuristic(const Planner::MinimalState & state,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, PDDL::PDDLStateFactory pddlFactory) {

    Planner::FF::STATES_EVALUATED++;
	writeTempStates(state, plan, timestamp, heuristic, pddlFactory);

	clock_t begin_time = clock();
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
	int planPos = result.find(H_PLAN_DELIM);
		if (planPos != -1) {
			string plan = result.substr(planPos + H_PLAN_DELIM.size(), pos);
			cout << plan << endl;
		}
	pos = result.find(H_VAL_DELIM);
	if (pos == -1) {
		cerr << "Problem was unsolvable - therefore max heuristic" << endl;
		return 1e5;
	}
	string h_val_str = result.substr(pos + H_VAL_DELIM.size());
	printf("%s\n", h_val_str.c_str());
	double hval = stod(h_val_str);
	planPos = result.find(H_PLAN_DELIM);
	if ((planPos != -1) && (hval == 0.0)) {
		string plan = result.substr(planPos + H_PLAN_DELIM.size(), pos);
		cout << plan << endl;
	}
	return hval;
}

void TRH::writeTempStates(const Planner::MinimalState & state,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, 
		PDDL::PDDLStateFactory pddlFactory) {
       clock_t begin_time = clock();

    /*Generate Domain*/

    //Domain
    PDDL::PDDLDomain domain = PDDL::PDDLDomainFactory::getInstance()->getDeTILedDomain(
    		VAL::current_analysis->the_domain, state, timestamp);

    //Shared data
    std::list<PDDL::Proposition> tilRequiredObjects = domain.getTILRequiredObjects();
    std::list<PDDL::Proposition> tilPredicates = domain.getTILPredicates();
    std::list<PDDL::Proposition> pendingActionRequiredObjects = domain.getPendingActionRequiredObjects();
    std::set<PDDL::PDDLObject> domainObjectSymbolTable = domain.getDomainObjectSymbolTable();

    //State
    PDDL::PDDLState pddlState = pddlFactory.getDeTILedPDDLState(state, plan, timestamp, 
        	heuristic, tilPredicates, tilRequiredObjects, pendingActionRequiredObjects, domainObjectSymbolTable);
        		
    TRH::TRH::TIME_SPENT_CONVERTING_PDDL_STATE += float( clock () - begin_time ) /  CLOCKS_PER_SEC;
        
    //Write State/Domain to disk for heuristic computation
    begin_time = clock();
	string filePath = "";
	string stateFileName = "temp";
	string domainFileName = "tempdomain";
	pddlState.writeDeTILedStateToFile(filePath, stateFileName);
	domain.writeToFile(filePath, domainFileName);
	TRH::TRH::TIME_SPENT_IN_PRINTING_TO_FILE += float( clock () - begin_time ) /  CLOCKS_PER_SEC;
}

}
