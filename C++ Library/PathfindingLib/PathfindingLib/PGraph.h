#pragma once
#include <unordered_map>
#include "PGraphNode.h"
#include "blaze\Blaze.h"

class PGraph
{
public:
	PGraph(blaze::StaticVector<double, 3UL> origin, blaze::StaticVector<double, 3UL> dimensions, double scale);
	~PGraph();
};

