#pragma once
#include "blaze\Blaze.h"

class PGraphNode
{
public:
	PGraphNode();
	~PGraphNode();
	blaze::StaticVector<double, 3UL> position;
	boolean obstacle;
	std::vector<PGraphNode*> neighbors;
};

