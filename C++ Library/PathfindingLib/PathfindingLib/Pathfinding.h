#pragma once
#include "Common.h"
#include "PGraph.h"
#include <vector>

class Pathfinding
{
public:
	static std::vector<Vector3>* a_star(PGraph* graph, Vector3 from, Vector3 to);
};

