/*
 * Proposition.h
 *
 *  Created on: 2 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PDDL_PROPOSITION_H_
#define COLIN_PDDL_PROPOSITION_H_

#include <list>
#include <map>
#include <string>
#include <iostream>

#include "PDDLObject.h"

namespace PDDL {

class Proposition {

private:
	std::string name;
	std::list<std::string> arguments;

public:
	Proposition(std::string name, std::list<std::string> arguments) : name(name), arguments(arguments) {};
	Proposition(){};
	const std::string & getName() const { return name; };
	const std::list<std::string> & getArguments() const {return arguments; };
	std::string toParameterisedString(const std::map<const PDDLObject *, std::string> & parameterTable) const;
	Proposition getParameterisedProposition(
			const std::map<const PDDLObject *, std::string> & parameterTable, bool showTypes = false) const;
	friend std::ostream & operator<<(std::ostream & output, const Proposition & proposition);
	bool operator==(const Proposition & other);
	static std::string getDecoratedName(const Proposition & proposition);
	friend std::string getDecoratedName(const Proposition & proposition);
};

}



#endif /* COLIN_PDDL_PROPOSITION_H_ */
