#pragma once
#include "PUnit.h"
#include <vector>
#include "blaze\Blaze.h"

class PUnitLayer
{
public:
	unsigned int id;
	unsigned int AddUnit(PUnit*);
	void RemoveUnit(unsigned int unit_id);
	void UpdateUnit(unsigned int unit_id, blaze::StaticVector<double, 3> position);
	std::vector<PUnit*> Nearby(unsigned int unit_id, double radius);
	PUnit* GetUnit(unsigned int unit_id);
private:

	std::vector<PUnit*> units;
};

