/*
 * PNEEffect.cpp
 *
 *  Created on: 3 May 2016
 *      Author: tony
 */

#include <vector>

#include "PNEEffect.h"
#include "PDDLUtils.h"
#include "ExpressionTree.h"

namespace PDDL {

std::ostream & operator<<(std::ostream & output,
		const PDDL::PNEEffect & pneEffect) {
	//Effect on fluent
	output << "(" << getAssignmentString(pneEffect.op);
	//Fluent Name
	output << " (" << pneEffect.name << ") ";
	//Convert equation for RPN (postfix) to PN (prefix)
	ExpressionTree * expTree = ExpressionTree::postfix2expTree(pneEffect.equation,
			pneEffect.parameterTable);
	output << ExpressionTree::preOrder(expTree->getExpRoot());
	//Closing parenthesis
	output << ")";
	return output;
}

}

