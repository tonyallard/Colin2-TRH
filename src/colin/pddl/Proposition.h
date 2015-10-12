/*
 * Proposition.h
 *
 *  Created on: 2 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PDDL_PROPOSITION_H_
#define COLIN_PDDL_PROPOSITION_H_

#include <list>
#include <string>
#include <iostream>

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
	friend std::ostream & operator<<(std::ostream & output, const Proposition & proposition);
	bool operator==(const Proposition & other);
	bool operator!=(const Proposition & other);
	static std::string getDecoratedName(const Proposition & proposition);
	friend std::string getDecoratedName(const Proposition & proposition);
};

}



#endif /* COLIN_PDDL_PROPOSITION_H_ */
