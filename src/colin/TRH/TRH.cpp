#include <stdlib.h>
#include <string>
#include <iostream>
#include <cstdio>
#include <memory>
#include <sstream>
#include <limits>
#include <random>

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
const char * TRH::H_CMD = "./lib/colin-clp";
const string TRH::H_VAL_DELIM = "State Heuristic Value is: ";
const string TRH::RELAXED_PLAN_SIZE_DELIM = "Relaxed plan length is: ";
const string TRH::H_STATES_EVAL_DELIM = "; States evaluated: ";
const string TRH::H_PLAN_DELIM = "(init-action)  [0.001]";
double TRH::TIME_SPENT_IN_HEURISTIC = 0.0;
double TRH::TIME_SPENT_IN_PRINTING_TO_FILE = 0.0;
double TRH::TIME_SPENT_CONVERTING_PDDL_STATE = 0.0;


TRH * TRH::getInstance() {
	if (!INSTANCE) {
		INSTANCE = new TRH(generateNewInstanceID());
	}
	return INSTANCE;
}

int TRH::generateNewInstanceID() {
	static std::random_device rd;
	static std::default_random_engine generator(rd());
	static std::uniform_int_distribution<int> distribution(0,
		std::numeric_limits<int>::max());
	return distribution(generator);
}

pair<double, int> TRH::getHeuristic(const Planner::MinimalState & state,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, PDDL::PDDLStateFactory pddlFactory) {

    Planner::FF::STATES_EVALUATED++;
	writeTempState(state, plan, timestamp, heuristic, pddlFactory);

	clock_t begin_time = clock();

	std::shared_ptr<FILE> pipe(popen(buildCommand().c_str(), "r"), pclose);
	if (!pipe)
		return std::make_pair(-1.0, -1.0);
	char buffer[128];
	std::string result = "";
	while (!feof(pipe.get())) {
		if (fgets(buffer, 128, pipe.get()) != NULL)
			result += buffer;
			 // cout << buffer;
	}
	TRH::TRH::TIME_SPENT_IN_HEURISTIC += double( clock () - begin_time ) /  CLOCKS_PER_SEC;
	int pos = result.find(H_STATES_EVAL_DELIM);
	if (pos != -1) {
		int posEnd = result.find("\n", pos);
		string statesEvalStr = result.substr(pos + H_STATES_EVAL_DELIM.size(), posEnd-(pos + H_STATES_EVAL_DELIM.size()));
		int statesEval = stoi(statesEvalStr);
		// printf("Heuristic States Eval: %s\n", statesEvalStr.c_str());
		Planner::FF::STATES_EVALUATED_IN_HEURISTIC += statesEval;
	}
	// int planPos = result.find(H_PLAN_DELIM);
	// 	if (planPos != -1) {
	// 		string plan = result.substr(planPos + H_PLAN_DELIM.size(), pos);
	// 		cout << plan << endl;
	// 	}
	pos = result.find(H_VAL_DELIM);
	if (pos == -1) {
		// cerr << "Problem was unsolvable - therefore heuristic value of -1.0" << endl;
		static int badStateNum = 0;
		writeBadState(state, plan, timestamp, heuristic, pddlFactory, badStateNum++);
		return std::make_pair(-1.0, -1.0);
	}
	string h_val_str = result.substr(pos + H_VAL_DELIM.size());
	// printf("%s\n", h_val_str.c_str());
	double hval = stod(h_val_str);
	
	int relaxedPlanSize = 0;
	pos = result.find(RELAXED_PLAN_SIZE_DELIM);
	if (pos != -1) {
		int posEnd = result.find("\n", pos);
		string relaxedPlanSizeStr = result.substr(pos + RELAXED_PLAN_SIZE_DELIM.size(), posEnd-(pos + RELAXED_PLAN_SIZE_DELIM.size()));
		relaxedPlanSize = stoi(relaxedPlanSizeStr);
		// printf("Relaxed Plan Length: %s\n", relaxedPlanSizeStr.c_str());
	}
	int planPos = result.find(H_PLAN_DELIM);
	if ((planPos != -1) && (hval == 0.0)) {
		string plan = result.substr(planPos + H_PLAN_DELIM.size(), pos);
		cout << plan << endl;
	}

	return std::make_pair (hval, relaxedPlanSize);
}

string TRH::buildCommand() {
	ostringstream cmd;
	cmd << H_CMD << " /tmp/temp" << TRH_INSTANCE_ID << "domain.pddl" 
		<< " /tmp/temp" << TRH_INSTANCE_ID << ".pddl";
	return cmd.str();
}

void TRH::writeTempState(const Planner::MinimalState & state,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, 
		PDDL::PDDLStateFactory pddlFactory) {
    // static int oneShot = 0;
    // if (oneShot++ > 1) {
    // 	cerr << "Exiting..." << endl;
    // 	exit(0);
    // }

	ostringstream stateFileName;
	stateFileName << "temp" << TRH_INSTANCE_ID;
	writeStateToFile(state, plan, timestamp, heuristic, 
		pddlFactory, stateFileName.str());
}

void TRH::writeBadState(const Planner::MinimalState & state,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, 
		PDDL::PDDLStateFactory pddlFactory, int stateNum) {
	
	ostringstream stateFileName;
	stateFileName << "BadState" << TRH_INSTANCE_ID << "-" << stateNum;
	writeStateToFile(state, plan, timestamp, heuristic, 
		pddlFactory, stateFileName.str());
}


void TRH::writeStateToFile(const Planner::MinimalState & state,
	std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, 
	PDDL::PDDLStateFactory pddlFactory, string fileName) {

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
        		
    TRH::TRH::TIME_SPENT_CONVERTING_PDDL_STATE += double( clock () - begin_time ) /  CLOCKS_PER_SEC;
        
    //Write State/Domain to disk for heuristic computation
    begin_time = clock();
	string filePath = "/tmp/";
	
	string domainFileName = fileName + "domain";
	pddlState.writeDeTILedStateToFile(filePath, fileName);
	domain.writeToFile(filePath, domainFileName);
	TRH::TRH::TIME_SPENT_IN_PRINTING_TO_FILE += double( clock () - begin_time ) /  CLOCKS_PER_SEC;

}
  
}
