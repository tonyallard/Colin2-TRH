/*
 * Literal.h
 *
 *  Created on: 2 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PDDL_LITERAL_H_
#define COLIN_PDDL_LITERAL_H_

#include <list>
#include <string>
#include <iostream>

namespace PDDL {

class Literal {

private:
	std::string name;
	std::list<std::string> arguments;

public:
	Literal(std::string name, std::list<std::string> arguments) : name(name), arguments(arguments) {};
	const std::string & getName() const { return name; };
	const std::list<std::string> & getArguments() const {return arguments; };
	friend std::ostream & operator<<(std::ostream & output, const Literal & literal);
};

}



#endif /* COLIN_PDDL_LITERAL_H_ */
