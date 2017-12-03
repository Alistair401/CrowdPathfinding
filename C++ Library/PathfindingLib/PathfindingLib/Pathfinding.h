#pragma once
#include "blaze\Blaze.h"
#include "PGraph.h"

class Pathfinding
{
public:
	static blaze::DynamicVector<blaze::StaticVector<float, 3UL>, blaze::rowVector> AStar(PGraph* graph, blaze::StaticVector<float, 3UL> from, blaze::StaticVector<float, 3UL> to);
};

