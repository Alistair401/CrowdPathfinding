#pragma once
#include "blaze\Blaze.h"
#include "PGraph.h"
#include <vector>

class Pathfinding
{
public:
	static std::vector<blaze::StaticVector<float, 3UL>>* a_star(PGraph* graph, blaze::StaticVector<float, 3> from, blaze::StaticVector<float, 3> to);
};

