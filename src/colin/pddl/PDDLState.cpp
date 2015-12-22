/*
 * PDDLState.cpp
 *
 *  Created on: 19 Jun 2015
 *      Author: tony
 */
#include <sstream>
#include <fstream>

#include "PDDLState.h"
#include "MMCRDomainFactory.h"

using namespace std;
namespace PDDL {

string PDDLState::toString() {
	ostringstream output;

	output << getLiteralString();
	output << getPNEString();

	// TILs to String
	list<TIL>::const_iterator tilItr = tils.begin();
	const list<TIL>::const_iterator tileItrEnd = tils.end();
	for (; tilItr != tileItrEnd; tilItr++) {
		output << *tilItr << "\n";
	}
	return output.str();
}

std::string PDDLState::getObjectSymbolTableString() {
	ostringstream output;
	set<PDDLObject>::const_iterator objItr = objectSymbolTable.begin();
	const set<PDDLObject>::const_iterator objItrEnd = objectSymbolTable.end();
	for (; objItr != objItrEnd; objItr++) {
		output << "\t\t" << *objItr << std::endl;
	}
	return output.str();
}

std::string PDDLState::getLiteralString() {
	ostringstream output;
	// Literals to String
	list<Proposition>::const_iterator litItr = literals.begin();
	const list<Proposition>::const_iterator litItrEnd = literals.end();
	for (; litItr != litItrEnd; litItr++) {
		output << "\t\t" << *litItr << "\n";
	}
	return output.str();
}

std::string PDDLState::getPNEString() {
	ostringstream output;
	// PNEs to String
	list<PNE>::const_iterator pneItr = pnes.begin();
	const list<PNE>::const_iterator pneItrEnd = pnes.end();
	for (; pneItr != pneItrEnd; pneItr++) {
		output << "\t\t" << *pneItr << "\n";
	}
	return output.str();
}

std::string PDDLState::getDomainString() {
	return MMCRDomainFactory::getMMCRDomain(pendingActions);
}

string PDDLState::getDeTiledDomainString() {
	return MMCRDomainFactory::getDeTILedMMCRDomain(tils, pendingActions);
}

string PDDLState::getPlanPrefixString() {
	ostringstream output;
	std::list<std::string>::const_iterator actionItr = planPrefix.begin();
	const std::list<std::string>::const_iterator actionItrEnd =
			planPrefix.end();
	for (; actionItr != actionItrEnd; actionItr++) {
		output << ";" << *actionItr << "\n";
	}
	return output.str();
}

std::string PDDLState::getTILObjectString() {
	ostringstream output;
	if (!tils.size()) {
		return output.str();
	}
	output << "\t\t";
	std::list<TIL>::const_iterator tilItr = tils.begin();
	for (; tilItr != tils.end(); tilItr++) {
		output << tilItr->getName() << " ";
	}
	output << "- TIL\n";
	return output.str();
}

std::string PDDLState::getGoalString() {
	ostringstream output;
	std::list<Proposition>::const_iterator goalItr = goals.begin();
	for (; goalItr != goals.end(); goalItr++) {
		output << *goalItr << " ";
	}
	return output.str();
}

std::string PDDLState::getTILGoalString() {
	ostringstream output;
	if (!tils.size()) {
		return output.str();
	}
	std::list<TIL>::const_iterator tilItr = tils.begin();
	for (; tilItr != tils.end(); tilItr++) {
		output << "(" << tilItr->getName() << ") ";
	}
	return output.str();
}

void PDDLState::writeStateToFile(string filePath, string fileName) {
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
	myFile << "\t(:objects" << std::endl << getObjectSymbolTableString()
			<< std::endl;
	myFile << "\t)\n";
	myFile << "\t(:init\n";
	myFile << toString();
	myFile << "\t)\n";
	myFile << "\t(:goal (and " << getGoalString() << "))\n";
	myFile << "\t(:metric " << metric << ")\n";
	myFile << ")";
	myFile.close();
}

void PDDLState::writeDeTILedStateToFile(std::string filePath,
		std::string fileName) {
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
	myFile << "\t(:objects" << std::endl << getObjectSymbolTableString()
			<< std::endl;
	myFile << "\t)\n";
	myFile << "\t(:init\n";
	myFile << getLiteralString() << getPNEString();
	myFile << "\t)\n";
	myFile << "\t(:goal (and " << getGoalString() << " " << getTILGoalString() << "))\n";
	myFile << "\t(:metric " << metric << ")\n";
	myFile << ")";
	myFile.close();
}

void PDDLState::writeDeTILedDomainToFile(string filePath, string fileName) {
	ofstream myFile;
	ostringstream fullFilePath;
	fullFilePath << filePath << fileName << "domain.pddl";
	myFile.open(fullFilePath.str());
	myFile << getDeTiledDomainString();
	myFile.close();
}

}
