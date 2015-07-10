/*
 * PNE.h
 *
 *  Created on: 2 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_PDDL_PNE_H_
#define COLIN_PDDL_PNE_H_

#include <list>
#include <initializer_list>
#include <string>

namespace PDDL {

class PNE {

private:
	std::string name;
	std::list<std::string> arguments;
	double value;

public:
	PNE(std::string name, std::initializer_list<std::string> arguments, double value) : name(name), arguments(arguments), value(value) {};
	PNE(std::string name, std::list<std::string> arguments, double value) : name(name), arguments(arguments), value(value) {};
	friend std::ostream & operator<<(std::ostream & output, const PNE & pne);
};

}



#endif /* COLIN_PDDL_PNE_H_ */
