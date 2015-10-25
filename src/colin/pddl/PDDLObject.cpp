/*
 * PDDLObject.cpp
 *
 *  Created on: 25 Oct 2015
 *      Author: tony
 */

#include "PDDLObject.h"

using namespace std;

bool PDDLObject::operator<(const PDDLObject & other) const {
	return name < other.name;
}

bool PDDLObject::operator==(const PDDLObject & other) {
	if (!(name.compare(other.name) + type.compare(other.type))) {
		return true;
	}
	return false;
}

ostream & operator<<(ostream & out, const PDDLObject & pddlObject) {
	out << pddlObject.name << " - " << pddlObject.type;
	return out;
}

