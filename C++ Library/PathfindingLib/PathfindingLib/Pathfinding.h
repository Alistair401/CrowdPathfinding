#pragma once
#include "blaze\Blaze.h"
#include "PGraph.h"
#include <vector>

class Pathfinding
{
public:
	static std::vector<blaze::StaticVector<double, 3UL>>* a_star(PGraph* graph, blaze::StaticVector<double, 3UL> from, blaze::StaticVector<double, 3UL> to);
};

