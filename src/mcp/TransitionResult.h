/*
 * TransitionResult.h
 *
 *  Created on: 14 Apr 2015
 *      Author: tony
 */

#ifndef MCP_TRANSITIONRESULT_H_
#define MCP_TRANSITIONRESULT_H_

namespace MCPlanning {
	class TransitionResult {
		private:
			int reward;
			Planner::MinimalState & nextState;
		public:
			inline int getReward() {
				return reward;
			};
			inline const Planner::MinimalState & getNextState() {
				return nextState;
			};
	};
};

#endif /* MCP_TRANSITIONRESULT_H_ */
