#pragma once
#include "Common.h"
#include <unordered_map>
#include "PGraphNode.h"
#include "blaze\Blaze.h"

class PGraph
{
public:
	PGraph(Vector3& origin, Vector3& dimensions, float scale);
	~PGraph();
	PGraphNode* PGraph::NodeAt(Vector3& position);
	PGraphNode* PGraph::NodeAtIndex(IVector3& index);
	std::unordered_map<IVector3, PGraphNode*, IVector3Hash>* graph;
private:
	Vector3 dimensions;
	Vector3 origin;
	float scale;
};

