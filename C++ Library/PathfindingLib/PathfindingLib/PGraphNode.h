#pragma once
#include "blaze\Blaze.h"

class PGraphNode
{
public:
	PGraphNode();
	~PGraphNode();
	blaze::StaticVector<double, 3UL> position;
	boolean obstacle;
private:
	blaze::StaticVector<int, 3UL> index;
};

