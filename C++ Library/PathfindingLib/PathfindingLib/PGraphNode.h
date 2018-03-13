#pragma once
#include "Common.h"

class PGraphNode
{
public:
	Vector3 position;
	IVector3 index;
	bool obstacle;
	std::vector<PGraphNode*> neighbors;
	bool operator==(PGraphNode& other) const {
		return this->index == other.index;
	}
};
