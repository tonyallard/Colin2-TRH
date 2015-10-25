/*
 * PDDLObject.h
 *
 *  Created on: 25 Oct 2015
 *      Author: tony
 */

#ifndef COLIN_PDDL_PDDLOBJECT_H_
#define COLIN_PDDL_PDDLOBJECT_H_

#include <iostream>
#include <string>

using namespace std;

class PDDLObject {
private:
	string name;
	string type;
public:
	PDDLObject(string name, string type) : name(name), type(type) {};
	string getName() const {return name;};
	string getType() const {return type;};

	bool operator<(const PDDLObject & other) const;
	bool operator==(const PDDLObject & other);

	friend ostream & operator<<(ostream & out, const PDDLObject & pddlObject);
};



#endif /* COLIN_PDDL_PDDLOBJECT_H_ */
