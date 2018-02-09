#pragma once
#include <unordered_map>
#include "PGraphNode.h"
#include "blaze\Blaze.h"

class PGraph
{
public:
	PGraph(blaze::StaticVector<float, 3>& origin, blaze::StaticVector<float, 3>& dimensions, float scale);
	~PGraph();
	PGraphNode* PGraph::NodeAt(blaze::StaticVector<float, 3>& position);
	PGraphNode* PGraph::NodeAtIndex(blaze::StaticVector<int, 3>& index);
	std::unordered_map<blaze::StaticVector<int, 3>, PGraphNode*, PGraphNode::IndexHash>* graph;
private:
	blaze::StaticVector<float, 3> dimensions;
	blaze::StaticVector<float, 3> origin;
	float scale;
};

