/*
 * Util.h
 *
 *  Created on: 27 Apr 2015
 *      Author: tony
 */

#ifndef UTIL_UTIL_H_
#define UTIL_UTIL_H_

#include "minimalstate.h"
#include "SearchQueueItem.h"
#include "instantiation.h"

namespace Planner {

void printAllLiterals();

void printState(const Planner::MinimalState & theState, double timeStamp);

void printLiteral(Inst::Literal * literal);

void printSearchNodeHeuristic(const SearchQueueItem * searchNode);

bool isSearchNodeValid(SearchQueueItem * searchNode);

}

#endif /* UTIL_UTIL_H_ */
