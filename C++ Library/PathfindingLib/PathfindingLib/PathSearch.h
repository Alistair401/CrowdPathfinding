#pragma once
#include "Common.h"
#include "PGraph.h"
#include <vector>

class PathSearch
{
public:
	static std::vector<Vector3>* AStar(PGraph* graph, Vector3 from, Vector3 to);
	static void ResetStats();
	static unsigned int EvaluatedCount();
	static unsigned int CallCount();
private:
	static unsigned int nodes_evaluated;
	static unsigned int a_star_calls;
};

