#pragma once
#include <unordered_map>
#include "PGraphNode.h"
#include "blaze\Blaze.h"

class PGraph
{
public:
	PGraph(blaze::StaticVector<double, 3UL> origin, blaze::StaticVector<double, 3UL> dimensions, double scale);
	~PGraph();
	PGraphNode* PGraph::NodeAt(blaze::StaticVector<double, 3UL> position);
private:
	std::unordered_map<blaze::StaticVector<int, 3UL>, PGraphNode*, PGraphNode::IndexHash>* graph;
	blaze::StaticVector<double, 3UL> dimensions;
	blaze::StaticVector<double, 3UL> origin;
	double scale;
};

