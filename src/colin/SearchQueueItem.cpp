/*
 * SearchQueueItem.cpp
 *
 *  Created on: 28 Apr 2015
 *      Author: tony
 */

#include "SearchQueueItem.h"
#include "globals.h"
#include "FFEvent.h"

ExtendedMinimalState * SearchQueueItem::releaseState() {
	assert (ownState);
	ownState = false;
	return internalState;
}

void SearchQueueItem::printPlan() {
	if (Globals::globalVerbosity & 2) {
		list<FFEvent>::iterator planItr = plan.begin();
		const list<FFEvent>::iterator planEnd = plan.end();

		for (int i = 0; planItr != planEnd; ++planItr, ++i) {
			if (!planItr->getEffects)
				cout << "(( ";
			if (planItr->action) {
				cout << i << ": " << *(planItr->action) << ", "
						<< (planItr->time_spec == VAL::E_AT_START ?
								"start" : "end");
			} else if (planItr->time_spec == VAL::E_AT) {
				cout << i << ": TIL " << planItr->divisionID;

			} else {
				cout << i << ": null node!";
				assert(false);
			}
			if (!planItr->getEffects)
				cout << " ))";
			cout << " at " << planItr->lpMinTimestamp;
			cout << "\n";
		}
	}
}
