#pragma once
#include "blaze\Blaze.h"

class PUnit
{
public:
	PUnit();
	~PUnit();
	blaze::StaticVector<float, 3UL> location;
	void UpdatePosition(blaze::StaticVector<float, 3UL> location);
};
