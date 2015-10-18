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

std::string PDDLState::getLiteralString() {
	ostringstream output;

	// Literals to String
	list<Proposition>::const_iterator litItr = literals.begin();
	const list<Proposition>::const_iterator litItrEnd = literals.end();
	for (; litItr != litItrEnd; litItr++) {
		output << *litItr << "\n";
	}
	return output.str();
}

std::string PDDLState::getPNEString() {
	ostringstream output;
	// PNEs to String
	list<PNE>::const_iterator pneItr = pnes.begin();
	const list<PNE>::const_iterator pneItrEnd = pnes.end();
	for (; pneItr != pneItrEnd; pneItr++) {
		output << *pneItr << "\n";
	}
	return output.str();
}

std::string PDDLState::getTILLiteralString() {
	ostringstream output;
	// TILs to Literals
	if (tils.size()) {
		list<TIL>::const_iterator tilItr = tils.begin();
		const list<TIL>::const_iterator tilItrEnd = tils.end();
		output << "\t\t\t";
		for (; tilItr != tilItrEnd; tilItr++) {
			output << tilItr->getName() << " ";
		}
		output << "- TIL\n";
	}
	return output.str();
}

std::string PDDLState::getDomainString() {
	return MMCRDomainFactory::getMMCRDomain(pendingActions);
}

string PDDLState::getDeTiledDomainString() {
	return MMCRDomainFactory::getDeTILedMMCRDomain(tils, pendingActions);
}

int PDDLState::getPreFixSize() {
	return planPrefix.size();
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
	myFile
			<< "\t(:objects \n\t\t v1 v2 - VEHICLE \n\t\t l1 l2 l3 - LOCATION \n\t\t c1  - CARGO \n\t)\n";
	myFile << "\t(:init\n";
	myFile << toString();
	myFile << "\t)\n";
	myFile << "\t(:goal (at c1 l3))\n";
	myFile << "\t(:metric minimize (total-cost))\n";
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
	myFile
			<< "\t(:objects \n\t\t v1 v2 - VEHICLE \n\t\t l1 l2 l3 - LOCATION \n\t\t c1  - CARGO \n";
	myFile << getTILLiteralString();
	myFile << "\t)\n";
	myFile << "\t(:init\n";
	myFile << getLiteralString() << getPNEString();
	myFile << "\t)\n";
	myFile << "\t(:goal (at c1 l3))\n";
	myFile << "\t(:metric minimize (total-cost))\n";
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

bool PDDLState::equals(const PDDLState & other) {
	//Check if the timestamp is the same
	if (this->timestamp != other.timestamp) {
		return false;
	}

	//Check if the literals are the same
	if (this->literals.size() != other.literals.size()) {
		return false;
	}
	std::list<Proposition>::const_iterator thisLitsItr = this->literals.begin();
	std::list<Proposition>::const_iterator othrLitsItr = other.literals.begin();
	for (; thisLitsItr != this->literals.end(); thisLitsItr++, othrLitsItr++) {
		Proposition thisProp = *thisLitsItr;
		Proposition othrProp = *othrLitsItr;
		if (thisProp != othrProp) {
			return false;
		}
	}
	//Check if the PNEs are the same
	if (this->pnes.size() != other.pnes.size()) {
		return false;
	}
	std::list<PNE>::const_iterator thisPNEsItr = this->pnes.begin();
	std::list<PNE>::const_iterator othrPNEsItr = other.pnes.begin();
	for (; thisPNEsItr != this->pnes.end(); thisPNEsItr++, othrPNEsItr++) {
		PNE thisPNE = *thisPNEsItr;
		PNE othrPNE = *othrPNEsItr;
		if (thisPNE != othrPNE) {
			return false;
		}
	}
	//Check if the TILs are the same
	if (this->tils.size() != other.tils.size()) {
		return false;
	}
	std::list<TIL>::const_iterator thisTILItr = this->tils.begin();
	std::list<TIL>::const_iterator othrTILItr = other.tils.begin();
	for (; thisTILItr != this->tils.end(); thisTILItr++, othrTILItr++) {
		TIL thisTIL = *thisTILItr;
		TIL othrTIL = *othrTILItr;
		if (thisTIL != othrTIL) {
			return false;
		}
	}
	//Check if the Pending Actions are the same
	if (this->pendingActions.size() != other.pendingActions.size()) {
		return false;
	}
	std::list<PendingAction>::const_iterator thisPAItr = this->pendingActions.begin();
	std::list<PendingAction>::const_iterator othrPAItr = other.pendingActions.begin();
	for (; thisPAItr != othrPAItr; thisPAItr++, othrPAItr++) {
		PendingAction thisPA = *thisPAItr;
		PendingAction othrPA = *othrPAItr;
		if (thisPA != othrPA) {
			return false;
		}
	}
	//They are the same
	return false;
}

}
