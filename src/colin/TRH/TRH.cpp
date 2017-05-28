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
#include "PDDLUtils.h"
#include "../FFSolver.h"
#include "instantiation.h"
#include "../temporalanalysis.h"

namespace TRH {

TRH * TRH::INSTANCE = NULL;
const char * TRH::H_CMD = "./lib/colin-clp";
const string TRH::TEMP_FILE_PATH = "";//"/tmp/";
const string TRH::TEMP_FILE_PREFIX = "temp";
const string TRH::TEMP_DOMAIN_SUFFIX = "-domain";
const string TRH::TEMP_FILE_EXT = ".pddl";
const string TRH::H_VAL_DELIM = "State Heuristic Value is: ";
const string TRH::RELAXED_PLAN_SIZE_DELIM = "Relaxed plan length is: ";
const string TRH::H_STATES_EVAL_DELIM = "; States evaluated: ";
const string TRH::H_PLAN_DELIM = "0.000:";
double TRH::TIME_SPENT_IN_HEURISTIC = 0.0;
double TRH::TIME_SPENT_IN_PRINTING_TO_FILE = 0.0;
double TRH::TIME_SPENT_CONVERTING_PDDL_STATE = 0.0;


TRH * TRH::getInstance() {
	if (!INSTANCE) {
		INSTANCE = new TRH(11);//generateNewInstanceID());
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

pair<double, int> TRH::getHeuristic(Planner::ExtendedMinimalState & theState,
		std::list<Planner::FFEvent>& plan, double timestamp, double heuristic, PDDL::PDDLStateFactory pddlFactory) {

	const Planner::MinimalState & state = theState.getInnerState();

    Planner::FF::STATES_EVALUATED++;
	string stateName = writeTempState(state, plan, timestamp, heuristic, pddlFactory);
	cout << "State Name: " << stateName << endl;
	clock_t begin_time = clock();

	std::shared_ptr<FILE> pipe(popen(buildCommand().c_str(), "r"), pclose);
	if (!pipe)
		return std::make_pair(-1.0, -1.0);
	char buffer[128];
	std::string result = "";
	while (!feof(pipe.get())) {
		if (fgets(buffer, 128, pipe.get()) != NULL)
			cout << buffer;
			result += buffer;
	}
	TRH::TRH::TIME_SPENT_IN_HEURISTIC += double( clock () - begin_time ) /  CLOCKS_PER_SEC;
	//removeTempState(stateName);
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
		string rPlan = result.substr(planPos, pos-planPos);
		list<Planner::FFEvent> relaexedPlan = getRelaxedPlan(rPlan, timestamp);
		list<Planner::FFEvent>::const_iterator rPlanItr = relaexedPlan.begin();
		for (; rPlanItr != relaexedPlan.end(); rPlanItr++) {
			evaluateStateAndUpdatePlan(*rPlanItr, theState, plan);
		}
	}
	return std::make_pair (hval, relaxedPlanSize);
}

string TRH::buildCommand() {
	ostringstream cmd;
	cmd << H_CMD << " " 
		<< TEMP_FILE_PATH << TEMP_FILE_PREFIX << TRH_INSTANCE_ID 
			<< TEMP_DOMAIN_SUFFIX << TEMP_FILE_EXT
		<< " " << TEMP_FILE_PATH << TEMP_FILE_PREFIX << TRH_INSTANCE_ID 
			<< TEMP_FILE_EXT;
	cout << cmd.str() << endl;
	return cmd.str();
}

list<Planner::FFEvent> TRH::getRelaxedPlan(string plan, double timestamp) {
	list<Planner::FFEvent> rPlan;
	string temp;
	std::istringstream inputStream (plan);
	int tilsEncountered = 0; //need to adjust timings for relaxed tils
	const double tilDuration = EPSILON;
	while (getline(inputStream, temp)) {
		
		int actionNameStartPos = temp.find("(");
		int actionNameEndPos = temp.find(")") + 1;
		string actionInstance = temp.substr(actionNameStartPos, 
			actionNameEndPos - actionNameStartPos);
		string actionName = actionInstance.substr(1, actionInstance.find(" ") - 1);
		Inst::instantiatedOp * op = PDDL::getOperator(actionInstance);
		
		int startTimePos = temp.find(":");
		double startTime = stod(temp.substr(0, startTimePos));
		//Add the timestamp to the action to fit after the prefix
		startTime += timestamp;
		//Deduct any previous TIL durations from startTimes
		startTime -= (tilsEncountered * tilDuration);

		int actionDurationStartPos = temp.find("[") + 1;
		int actionDurationEndPos = temp.find("]");
		double duration = stod(temp.substr(actionDurationStartPos, 
			actionDurationEndPos - actionDurationStartPos));

		if (op == 0){
			//Is it a til action?
			//FIXME: Possibly need to be more thorough here
			if ((PDDL::isTILAction(actionName, 0.0, 0.0)) && 
				(duration == EPSILON)){
				tilsEncountered++;
			}
		} else {
			if (duration > EPSILON) {
				//Durative Action
				Planner::FFEvent start_snap_action = Planner::FFEvent(op, 
					duration, duration);
				start_snap_action.time_spec = VAL::E_AT_START;
				rPlan.push_back(start_snap_action);
			} else {
				Planner::FFEvent start_snap_action = Planner::FFEvent(op, 
					duration, duration);
				start_snap_action.time_spec = VAL::E_AT_START;
				rPlan.push_back(start_snap_action);
			}
			
		}
	}
	return rPlan;
}

void TRH::evaluateStateAndUpdatePlan(const Planner::FFEvent & actionToBeApplied,
		Planner::ExtendedMinimalState & state, list<Planner::FFEvent> & plan)
{

	list<Planner::ActionSegment> helpfulActions;


	Planner::FFEvent extraEvent;
	Planner::FFEvent extraEventTwo;

    bool eventOneDefined = false;
    bool eventTwoDefined = false;

    const bool rawDebug = false;

    map<double, list<pair<int, int> > > * justApplied = 0;
    map<double, list<pair<int, int> > > actualJustApplied;
    double tilFrom = 0.001;

    int stepID = -1;

    if (actionToBeApplied.time_spec == VAL::E_AT_START) {
        if (rawDebug) cout << "RAW start\n";
        extraEvent = Planner::FFEvent(actionToBeApplied.action, state.startEventQueue.back().minDuration, state.startEventQueue.back().maxDuration);
        eventOneDefined = true;

        assert(extraEvent.time_spec == VAL::E_AT_START);
        Planner::FF::makeJustApplied(actualJustApplied, tilFrom, state, true);
        if (!actualJustApplied.empty()) justApplied = &actualJustApplied;

        if (!Planner::RPGBuilder::getRPGDEs(actionToBeApplied.action->getID()).empty()) { // if it's not a non-temporal action

            const int endStepID = state.getInnerState().planLength - 1;
            const int startStepID = endStepID - 1;
            extraEventTwo = Planner::FFEvent(actionToBeApplied.action, startStepID, state.startEventQueue.back().minDuration, state.startEventQueue.back().maxDuration);
            extraEvent.pairWithStep = endStepID;
            eventTwoDefined = true;

            if (!Planner::TemporalAnalysis::canSkipToEnd(actionToBeApplied.action->getID())) {
                extraEventTwo.getEffects = false;
            }

            stepID = startStepID;
        } else {
            stepID = state.getInnerState().planLength - 1;
        }
    } else {
        extraEvent = Planner::FFEvent(actionToBeApplied.divisionID);
        eventOneDefined = true;
        stepID = state.getInnerState().planLength - 1;
    }


    list<Planner::FFEvent> nowList;

    if (eventOneDefined) nowList.push_back(extraEvent);
    if (eventTwoDefined) nowList.push_back(extraEventTwo);

    assert(stepID != -1);

    if (eventTwoDefined) {
        extraEventTwo = nowList.back();
        nowList.pop_back();
    }

    if (eventOneDefined) {
        extraEvent = nowList.back();
    }

    #ifdef POPF3ANALYSIS
    #ifndef TOTALORDERSTATES
    if (actionToBeApplied.time_spec == VAL::E_AT_START) {
        if (   !RPGBuilder::getRPGDEs(actionToBeApplied.action->getID()).empty()
            && TemporalAnalysis::canSkipToEnd(actionToBeApplied.action->getID())) { // if it's a compression-safe temporal action

            const int endStepID = state.getInnerState().planLength - 1;

            state.getEditableInnerState().updateWhenEndOfActionIs(actionToBeApplied.action->getID(), endStepID, extraEventTwo.lpMinTimestamp);

        }
    }
    #endif
    #endif

    if (eventOneDefined) {
        plan.push_back(extraEvent);
    }

    if (eventTwoDefined) {
        plan.push_back(extraEventTwo);
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
	stateFileName << TEMP_FILE_PREFIX << TRH_INSTANCE_ID;
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
	
	string domainFileName = fileName + TEMP_DOMAIN_SUFFIX;
	pddlState.writeDeTILedStateToFile(TEMP_FILE_PATH, fileName);
	domain.writeToFile(TEMP_FILE_PATH, domainFileName);
	TRH::TRH::TIME_SPENT_IN_PRINTING_TO_FILE += double( clock () - begin_time ) /  CLOCKS_PER_SEC;
}

void TRH::removeTempState(string fileName) {
	ostringstream stateFileName;
	ostringstream domainFileName;

	domainFileName << TEMP_FILE_PATH << fileName
		<< TEMP_DOMAIN_SUFFIX << TEMP_FILE_EXT;
	stateFileName << TEMP_FILE_PATH << fileName << TEMP_FILE_EXT;

	//Remove Domain File
	remove(domainFileName.str().c_str());
	//Remove State File
	remove(stateFileName.str().c_str());
}
  
}
