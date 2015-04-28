/*
 * ExtendedMinimalState.h
 *
 *  Created on: 28 Apr 2015
 *      Author: tony
 */

#ifndef COLIN_EXTENDEDMINIMALSTATE_H_
#define COLIN_EXTENDEDMINIMALSTATE_H_

#include "StartEvent.h"
#include "minimalstate.h"

#include <map>
#include <vector>
#include <set>

using std::map;
using std::vector;
using std::set;

namespace Planner{

class ExtendedMinimalState
{

private:
    bool operator ==(ExtendedMinimalState &);

protected:
    MinimalState * decorated;

    ExtendedMinimalState(const ExtendedMinimalState & e)
    : decorated(new MinimalState(*(e.decorated))), startEventQueue(e.startEventQueue), timeStamp(e.timeStamp), stepBeforeTIL(e.stepBeforeTIL), tilFanIn(e.tilFanIn), tilComesBefore(e.tilComesBefore)  {

//      factsIfWeFinishActions = e.factsIfWeFinishActions;

        list<StartEvent>::iterator bqItr = startEventQueue.begin();
        const list<StartEvent>::iterator bqEnd = startEventQueue.end();

        for (; bqItr != bqEnd; ++bqItr) {
            entriesForAction[bqItr->actID].push_back(bqItr);
        }

    }

    ExtendedMinimalState & operator=(const ExtendedMinimalState & e);

public:

    list<StartEvent> startEventQueue;
    map<int, list<list<StartEvent>::iterator > > entriesForAction;

    double timeStamp;
    int stepBeforeTIL;
    int tilFanIn;
    list<int> tilComesBefore;
    bool hasBeenDominated;

    ExtendedMinimalState(const set<int> & f, const vector<double> & sMin, const vector<double> & sMax, const map<int, set<int> > & sa, const double & ts, const int & nt, const unsigned int & pl)
        : decorated(new MinimalState(f, sMin, sMax, sa, nt, pl)), timeStamp(ts), stepBeforeTIL(-1), tilFanIn(0), hasBeenDominated(false) {
    }

    ExtendedMinimalState()
        : decorated(new MinimalState()), timeStamp(0.0), stepBeforeTIL(-1), tilFanIn(0), hasBeenDominated(false) {
    }



    ExtendedMinimalState(const ExtendedMinimalState & e, MinimalState * const ms)
        : decorated(ms), startEventQueue(e.startEventQueue), timeStamp(e.timeStamp),
          stepBeforeTIL(e.stepBeforeTIL), tilFanIn(e.tilFanIn), tilComesBefore(e.tilComesBefore),
          hasBeenDominated(e.hasBeenDominated)  {

        //      factsIfWeFinishActions = e.factsIfWeFinishActions;

        list<StartEvent>::iterator bqItr = startEventQueue.begin();
        const list<StartEvent>::iterator bqEnd = startEventQueue.end();

        for (; bqItr != bqEnd; ++bqItr) {
            entriesForAction[bqItr->actID].push_back(bqItr);
        }

    }


    virtual ~ExtendedMinimalState() {
#ifdef STATEHASHDEBUG
        cout << "Deleting state at " << decorated << std::endl;
#endif
        delete decorated;
    }

    static bool queueEqual(const list<StartEvent> & a, const list<StartEvent> & b);

//  virtual bool operator==(const ExtendedMinimalState & o) const {
//      return (nextTIL == o.nextTIL && first == o.first && secondMin == o.secondMin && secondMax == o.secondMax && startedActions == o.startedActions && invariants == o.invariants && fluentInvariants == o.fluentInvariants && stepBeforeTIL == o.stepBeforeTIL && tilFanIn == o.tilFanIn && tilComesBefore == o.tilComesBefore && queueEqual(startEventQueue, o.startEventQueue) && fabs(timeStamp - o.timeStamp) < 0.0005);
//  }

    virtual void deQueueFirstOf(const int & actID, const int & divID);
    virtual void deQueueStep(const int & actID, const int & stepID);

    MinimalState & getEditableInnerState();
    const MinimalState & getInnerState() const;
    ExtendedMinimalState * applyAction(const ActionSegment & a, const vector<double> & minTimestamps, double minDur = 0.0, double maxDur = 0.0) const;
    void applyActionLocally(const ActionSegment & a, const vector<double> & minTimestamps, double minDur = 0.0, double maxDur = 0.0);
    ExtendedMinimalState * clone() const;
};
}



#endif /* COLIN_EXTENDEDMINIMALSTATE_H_ */
