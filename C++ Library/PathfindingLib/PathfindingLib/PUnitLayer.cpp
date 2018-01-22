#include "stdafx.h"
#include "PUnitLayer.h"
#include "blaze\Blaze.h"

unsigned int PUnitLayer::AddUnit(PUnit * unit)
{
	unsigned int index = static_cast<unsigned int>(units.size());
	units.push_back(unit);
	return index;
}

void PUnitLayer::RemoveUnit(unsigned int unit_id)
{
	units[unit_id] = nullptr;
}

std::vector<PUnit*> PUnitLayer::Nearby(unsigned int unit_id, double radius)
{
	PUnit* current = units.at(unit_id);
	std::vector<PUnit*> result;
	for (size_t i = 0; i < units.size(); i++)
	{
		if (units.at(i) == nullptr) continue;
		if (static_cast<unsigned int>(i) == unit_id) continue;
		PUnit* u = units.at(i);
		blaze::StaticVector<double, 3> vec = u->GetPosition() - current->GetPosition();
		double dist = blaze::length(vec);
		if (dist < radius) result.push_back(u);
	}
	return result;
}

PUnit * PUnitLayer::GetUnit(unsigned int unit_id)
{
	return units.at(unit_id);
}
