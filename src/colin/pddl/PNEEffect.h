/*
 * MathematicalEffect.h
 *
 *  Created on: 3 May 2016
 *      Author: tony
 */

#ifndef COLIN_PDDL_PNEEFFECT_H_
#define COLIN_PDDL_PNEEFFECT_H_

#include <string>
#include <list>
#include <map>

#include "PDDLObject.h"
#include "../RPGBuilder.h"

using namespace std;

namespace PDDL {

class PNEEffect {

private:
	VAL::assign_op op;
	string name;
	list<Planner::RPGBuilder::Operand> equation;
	map<PDDLObject, string> parameterTable;

public:
	PNEEffect(string name, VAL::assign_op op, list<Planner::RPGBuilder::Operand> equation) :
		name(name), op(op), equation(equation) {
	}
	;
	friend std::ostream & operator<<(std::ostream & output, const PDDL::PNEEffect & pneEffect);

};
}
#endif /* COLIN_PDDL_PNEEFFECT_H_ */
