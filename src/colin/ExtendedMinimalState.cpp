/*
 * ExtendedMinimalState.cpp
 *
 *  Created on: 28 Apr 2015
 *      Author: tony
 */

#include "ExtendedMinimalState.h"
#include <assert.h>

using namespace Planner;

MinimalState & ExtendedMinimalState::getEditableInnerState() {
	return *decorated;
}

const MinimalState & ExtendedMinimalState::getInnerState() const {
	return *decorated;
}

ExtendedMinimalState * ExtendedMinimalState::applyAction(
		const ActionSegment & a, const vector<double> & minTimestamps,
		double minDur, double maxDur) const {
	return new ExtendedMinimalState(*this,
			decorated->applyAction(a, minTimestamps, minDur, maxDur));
}

void ExtendedMinimalState::applyActionLocally(const ActionSegment & a,
		const vector<double> & minTimestamps, double minDur, double maxDur) {
	decorated->applyActionLocally(a, minTimestamps, minDur, maxDur);
}

ExtendedMinimalState * ExtendedMinimalState::clone() const {
	return new ExtendedMinimalState(*this);
}

bool ExtendedMinimalState::queueEqual(const list<StartEvent> & a,
		const list<StartEvent> & b) {
	list<StartEvent>::const_iterator aItr = a.begin();
	const list<StartEvent>::const_iterator aEnd = a.end();

	list<StartEvent>::const_iterator bItr = b.begin();
	const list<StartEvent>::const_iterator bEnd = b.end();

	for (; aItr != aEnd && bItr != bEnd; ++aItr, ++bItr) {
		if (!(*aItr == *bItr))
			return false;
	}

	return ((aItr == aEnd) == (bItr == bEnd));

}

bool ExtendedMinimalState::operator ==(ExtendedMinimalState &) {
	assert(false);
	return false;
}
