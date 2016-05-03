/*
 * PNEEffect.cpp
 *
 *  Created on: 3 May 2016
 *      Author: tony
 */

#include "PNEEffect.h"

#include "PDDLUtils.h"

namespace PDDL {

std::ostream & operator<<(std::ostream & output, const PNEEffect & pneEffect) {
	//Effect on fluent
	output << "(" << getAssignmentString(pneEffect.op);
	//Fluent Name
	output << "(" << pneEffect.name << ") ";

	//Store numbers while converting from postfix to PNF
	list<Planner::RPGBuilder::Operand> numberStack;
	int stackPtr = 0;

	list<Planner::RPGBuilder::Operand>::const_iterator opsItr =
			pneEffect.equation.begin();

	for (; opsItr != pneEffect.equation.end(); opsItr++) {
		if ((opsItr->numericOp == Planner::RPGBuilder::math_op::NE_CONSTANT) ||
				(opsItr->numericOp == Planner::RPGBuilder::math_op::NE_CONSTANT))
		{
			numberStack.push_front(*opsItr);
		} else {
			cout << "(" << getOperandString(*opsItr, pneEffect.parameterTable) << " ";
			cout << getOperandString(numberStack[0], pneEffect.parameterTable) << " ";
			numberStack.pop_front();
			cout << getOperandString(numberStack[0], pneEffect.parameterTable) << ") ";
			numberStack.pop_front();
		}
	}
}

}

