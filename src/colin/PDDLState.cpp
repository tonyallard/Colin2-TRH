/*
 * PDDLState.cpp
 *
 *  Created on: 19 Jun 2015
 *      Author: tony
 */
#include <sstream>
#include <fstream>

#include "PDDLState.h"
#include "Util.h"

using namespace std;

Planner::PDDLState::PDDLState(const MinimalState & state, double timeStamp, double heuristic) {
	literals = Planner::getStateLiteralsString(state);
	fluents = Planner::getStateFluentString(state);
	tils = Planner::getStateTILString(state, timeStamp);
	metric = Planner::getMetricString();
	this->timeStamp = timeStamp;
	this->heuristic = heuristic;
}

string Planner::PDDLState::toString() {
	ostringstream output;
	output << literals << fluents << tils << metric;
	return output.str();
}

void Planner::PDDLState::writeToFile(string fileName) {
	ofstream myFile;
	myFile.open(fileName.c_str());
	myFile << ";time stamp: " << timeStamp << "\n";
	myFile << ";heuristic: " << heuristic << "\n";
	myFile << "(define (problem " << fileName << ")\n";
	myFile << "\t(:domain multi-modal-cargo-routing)\n";
	myFile << "\t(:objects \n\t\t V1 V2 - VEHICLE \n\t\t L1 L2 L3 - LOCATION \n\t\t C1  - CARGO \n\t)\n";
	myFile << "\t(:init\n";
	myFile << toString();
	myFile << "\t)\n";
	myFile << "\t(:goal (at C1 L3))\n";
	myFile << "\t(:metric minimize (total-cost))\n";
	myFile << ")";
	myFile.close();
}


