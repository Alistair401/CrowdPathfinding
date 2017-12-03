#pragma once
#include <unordered_map>
#include "PGraphNode.h"
#include "blaze\Blaze.h"

class PGraph
{
public:
	PGraph(blaze::StaticVector<float, 3UL> origin, blaze::StaticVector<int, 3UL> dimensions, float scale);
	~PGraph();
};

