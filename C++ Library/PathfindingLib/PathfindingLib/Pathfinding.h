#pragma once
#include "blaze\Blaze.h"
#include "PGraph.h"

class Pathfinding
{
public:
	static blaze::DynamicVector<blaze::StaticVector<double, 3UL>, blaze::rowVector>* a_star(PGraph* graph, blaze::StaticVector<double, 3UL> from, blaze::StaticVector<double, 3UL> to);
};

