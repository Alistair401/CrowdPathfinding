#pragma once
#include "PUnit.h"
#include <vector>

class PUnitLayer
{
public:
	unsigned int id;
	unsigned int AddUnit(PUnit*);
	void RemoveUnit(unsigned int unit_id);
	std::vector<PUnit*> Nearby(unsigned int unit_id, double radius);
	PUnit* GetUnit(unsigned int unit_id);
private:
	std::vector<PUnit*> units;
};

