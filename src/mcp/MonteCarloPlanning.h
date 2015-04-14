#ifndef MCP__MONTECARLOPLANNING_H_
#define MCP__MONTECARLOPLANNING_H_

namespace MCPlanning
{
	class MonteCarloPlanning {
	
	private:
		static int search(const Planner::MinimalState & state, int depth);
		static const Planner::ActionSegment & selectAction(const Planner::MinimalState & state, int depth);
		static const TransitionResult & simulateAction(const Planner::MinimalState & state, Planner::ActionSegment & action);
		static double gamma = 0.1;
	
	public:
		static int MonteCarloPlanning(const Planner::MinimalState & state);
	};
};

#endif
