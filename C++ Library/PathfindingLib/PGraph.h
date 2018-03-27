#pragma once
#include "Common.h"
#include <unordered_map>
#include "PGraphNode.h"
#include <vector>

class PGraph
{
public:
	PGraph(Vector3& origin, Vector3& dimensions, float scale);
	~PGraph();
	PGraphNode* PGraph::NodeAt(Vector3& position);
	PGraphNode* PGraph::NodeAtIndex(IVector3& index);
	std::vector<std::vector<std::vector<PGraphNode*>>> grid;
private:
	Vector3 dimensions;
	Vector3 origin;
	float scale;
};

