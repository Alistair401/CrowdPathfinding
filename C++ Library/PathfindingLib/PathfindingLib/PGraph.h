#pragma once
#include <unordered_map>
#include "PGraphNode.h"
#include "blaze\Blaze.h"

class PGraph
{
public:
	PGraph(blaze::StaticVector<double, 3> origin, blaze::StaticVector<double, 3> dimensions, double scale);
	~PGraph();
	PGraphNode* PGraph::NodeAt(blaze::StaticVector<double, 3> position);
private:
	std::unordered_map<blaze::StaticVector<int, 3>, PGraphNode*, PGraphNode::IndexHash>* graph;
	blaze::StaticVector<double, 3> dimensions;
	blaze::StaticVector<double, 3> origin;
	double scale;
};

