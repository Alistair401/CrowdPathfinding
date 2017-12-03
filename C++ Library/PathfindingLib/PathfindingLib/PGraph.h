#pragma once
#include <unordered_map>
#include "PGraphNode.h"

class PGraph
{
public:
	PGraph(blaze::StaticVector<float, 3UL> origin, blaze::StaticVector<int, 3UL> dimensions, float scale);
	~PGraph();
private:
	struct IndexVectorEquals;
	struct HashIndexVector;
	std::unordered_map<blaze::StaticVector<int, 3UL>, PGraphNode, HashIndexVector, IndexVectorEquals> graph;
};

