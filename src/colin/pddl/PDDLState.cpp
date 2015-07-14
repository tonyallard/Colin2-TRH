/*
 * PDDLState.cpp
 *
 *  Created on: 19 Jun 2015
 *      Author: tony
 */
#include <sstream>
#include <fstream>

#include "PDDLState.h"

using namespace std;
namespace PDDL {

string PDDLState::toString() {
	ostringstream output;

	// Literals to String
	list<Literal>::const_iterator litItr = literals.begin();
	const list<Literal>::const_iterator litItrEnd = literals.end();
	for (; litItr != litItrEnd; litItr++) {
		output << *litItr << "\n";
	}

	// PNEs to String
	list<PNE>::const_iterator pneItr = pnes.begin();
	const list<PNE>::const_iterator pneItrEnd = pnes.end();
	for (; pneItr != pneItrEnd; pneItr++) {
		output << *pneItr << "\n";
	}

	// TILs to String
	list<TIL>::const_iterator tilItr = tils.begin();
	const list<TIL>::const_iterator tileItrEnd = tils.end();
	for (; tilItr != tileItrEnd; tilItr++) {
		output << *tilItr << "\n";
	}
	return output.str();
}

string PDDLState::getPlanPrefixString() {
	ostringstream output;
	std::list<std::string>::const_iterator actionItr = planPrefix.begin();
	const std::list<std::string>::const_iterator actionItrEnd = planPrefix.end();
	for (; actionItr != actionItrEnd; actionItr++) {
		output << ";" << *actionItr << "\n";
	}
	return output.str();
}

void PDDLState::writeToFile(string filePath, string fileName) {
	ofstream myFile;
	ostringstream fullFilePath;
	fullFilePath << filePath << fileName << ".pddl";
	myFile.open(fullFilePath.str());
	myFile << ";time stamp: " << timestamp << "\n";
	myFile << ";heuristic: " << heuristic << "\n";
	myFile << ";plan prefix\n";
	myFile << getPlanPrefixString() << "\n";
	myFile << "(define (problem " << fileName << ")\n";
	myFile << "\t(:domain multi-modal-cargo-routing)\n";
	myFile
			<< "\t(:objects \n\t\t V1 V2 - VEHICLE \n\t\t L1 L2 L3 - LOCATION \n\t\t C1  - CARGO \n\t)\n";
	myFile << "\t(:init\n";
	myFile << toString();
	myFile << "\t)\n";
	myFile << "\t(:goal (at C1 L3))\n";
	myFile << "\t(:metric minimize (total-cost))\n";
	myFile << ")";
	myFile.close();
}
}
