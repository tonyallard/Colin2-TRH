/*
 * FakeTILAction.h
 *
 *  Created on: 8 Jul 2015
 *      Author: tony
 */

#ifndef COLIN_FAKETILACTION_H_
#define COLIN_FAKETILACTION_H_

#include <list>
#include "instantiation.h"
#include "globals.h"

using namespace std;

namespace Planner {

/**
 *  Class to represent the 'action' whose application corresponds to a timed initial literal.
 */
class FakeTILAction {

public:
	/** The time-stamp of the timed initial literal */
	const double duration;

	/** The facts added at the specified time */
	list<Inst::Literal*> addEffects;

	/** The facts deleted at the specified time */
	list<Inst::Literal*> delEffects;

	/**
	 *   Add the specified add and delete effects to the timed initial literal action.  Is used when
	 *   multiple TILs are found at a given time-stamp.
	 *
	 *   @param adds  Add effects to include in this TIL action
	 *   @param dels  Delete effects to include in this TIL action
	 */
	void mergeIn(const Planner::LiteralSet & adds, const Planner::LiteralSet & dels) {

		{
			Planner::LiteralSet::iterator lsItr = adds.begin();
			const Planner::LiteralSet::iterator lsEnd = adds.end();

			for (; lsItr != lsEnd; ++lsItr) {
				addEffects.push_back(*lsItr);
			}
		}

		{
			Planner::LiteralSet::iterator lsItr = dels.begin();
			const Planner::LiteralSet::iterator lsEnd = dels.end();

			for (; lsItr != lsEnd; ++lsItr) {
				delEffects.push_back(*lsItr);
			}
		}
	}

	/**
	 *   Constructor for an action corresponding to a Timed Initial Literal.
	 *
	 *   @param dur  The time at which the timed initial occurs
	 *   @param adds The facts added at time <code>dur</code>
	 *   @param dels The facts deleted at time <code>dur</code>
	 */
	FakeTILAction(const double & dur, const Planner::LiteralSet & adds,
			const Planner::LiteralSet & dels) :
			duration(dur) {
		mergeIn(adds, dels);
	}

};

}

#endif /* COLIN_FAKETILACTION_H_ */
