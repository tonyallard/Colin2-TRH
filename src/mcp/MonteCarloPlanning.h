#ifndef __MONTECARLOPLANNING
#define __MONTECARLOPLANNING

namespace MCPlanning
{
	class MonteCarloPlanning {
	
	private:
		static int search(const Planner::MinimalState & state, int depth);
	
	public:
		static int MonteCarloPlanning(const Planner::MinimalState & state);
	};
};

#endif
