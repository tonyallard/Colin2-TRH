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
const string TRH::H_PLAN_DELIM = "0.000:";
double TRH::TIME_SPENT_IN_HEURISTIC = 0.0;
double TRH::TIME_SPENT_IN_PRINTING_TO_FILE = 0.0;
double TRH::TIME_SPENT_CONVERTING_PDDL_STATE = 0.0;
list<Util::triple<double, string, double> > TRH::RELAXED_PLAN;


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
	string stateName = writeTempState(state, plan, timestamp, heuristic, pddlFactory);

	clock_t begin_time = clock();

	std::shared_ptr<FILE> pipe(popen(buildCommand().c_str(), "r"), pclose);
	if (!pipe)
		return std::make_pair(-1.0, -1.0);
	char buffer[128];
	std::string result = "";
	while (!feof(pipe.get())) {
		if (fgets(buffer, 128, pipe.get()) != NULL)
			result += buffer;
	}
	TRH::TRH::TIME_SPENT_IN_HEURISTIC += double( clock () - begin_time ) /  CLOCKS_PER_SEC;
	removeTempState(stateName);
	int pos = result.find(H_STATES_EVAL_DELIM);
	if (pos != -1) {
		int posEnd = result.find("\n", pos);
		string statesEvalStr = result.substr(pos + H_STATES_EVAL_DELIM.size(), posEnd-(pos + H_STATES_EVAL_DELIM.size()));
		int statesEval = stoi(statesEvalStr);
		// printf("Heuristic States Eval: %s\n", statesEvalStr.c_str());
		Planner::FF::STATES_EVALUATED_IN_HEURISTIC += statesEval;
	}
	pos = result.find(H_VAL_DELIM);
	if (pos == -1) {
		// cerr << "Problem was unsolvable - therefore heuristic value of -1.0" << endl;
		static int badStateNum = 0;
		//writeBadState(state, plan, timestamp, heuristic, pddlFactory, badStateNum++);
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
		string plan = result.substr(planPos, pos-planPos);
		RELAXED_PLAN = getRelaxedPlan(plan, timestamp);
	}

	return std::make_pair (hval, relaxedPlanSize);
}

string TRH::buildCommand() {
	ostringstream cmd;
	cmd << H_CMD << " /tmp/temp" << TRH_INSTANCE_ID << "domain.pddl" 
		<< " /tmp/temp" << TRH_INSTANCE_ID << ".pddl";
	return cmd.str();
}

list<Util::triple<double, string, double> > TRH::getRelaxedPlan(string plan, double timestamp) {
	list<Util::triple<double, string, double> > rPlan;
	string temp;
	std::istringstream inputStream (plan);
	int tilsEncountered = 0; //need to adjust timings for relaxed tils
	const double tilDuration = EPSILON;
	while (getline(inputStream, temp)) {
		
		int actionNameStartPos = temp.find("(");
		int actionNameEndPos = temp.find(")") + 1;
		string actionInstance = temp.substr(actionNameStartPos, actionNameEndPos - actionNameStartPos);
		string actionName = actionInstance.substr(1, actionInstance.find(" ") - 1);
		
		if (PDDL::PDDLDomainFactory::getInstance()->isDomainOperator(actionName)){
			int startTimePos = temp.find(":");
			
			double startTime = stod(temp.substr(0, startTimePos));
			//Add the timestamp to the action to fit after the prefix
			startTime += timestamp;
			//Deduct any previous TIL durations from startTimes
			startTime -= (tilsEncountered * tilDuration);

			int actionDurationStartPos = temp.find("[") + 1;
			int actionDurationEndPos = temp.find("]");
			double duration = stod(temp.substr(actionDurationStartPos, actionDurationEndPos - actionDurationStartPos));
			Util::triple<double, string, double> action;
			action.make_triple(startTime, actionInstance, duration);
			rPlan.push_back(action);
		} else {
			//assume TIL action
			//FIXME: Possibly need to be more thorough here
			tilsEncountered++;
		}
	}
	return rPlan;
}

void TRH::printPlanPostfix() {
	list<Util::triple<double, string, double> >::const_iterator rPlanItr =
		RELAXED_PLAN.begin();
	for (; rPlanItr != RELAXED_PLAN.end(); rPlanItr++) {
		cout << rPlanItr->first << ": " << rPlanItr->second << "  [" <<
			rPlanItr->third << "]" << endl;
	}
}

string TRH::writeTempState(const Planner::MinimalState & state,
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
	return stateFileName.str();
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
	static const string filePath = "/tmp/";
	
	string domainFileName = fileName + "domain";
	pddlState.writeDeTILedStateToFile(filePath, fileName);
	domain.writeToFile(filePath, domainFileName);
	TRH::TRH::TIME_SPENT_IN_PRINTING_TO_FILE += double( clock () - begin_time ) /  CLOCKS_PER_SEC;
}

void TRH::removeTempState(string fileName) {
	static const string filePath = "/tmp/";
	static const string extension = ".pddl";
	ostringstream stateFileName;
	ostringstream domainFileName;

	domainFileName << filePath << fileName
		<< "domain" << extension;
	stateFileName << filePath << fileName 
		<< extension;

	//Remove Domain File
	remove(domainFileName.str().c_str());
	//Remove State File
	remove(stateFileName.str().c_str());
}
  
}
