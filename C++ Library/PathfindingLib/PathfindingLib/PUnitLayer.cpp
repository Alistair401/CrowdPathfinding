#include "stdafx.h"
#include "PUnitLayer.h"
#include <boost/geometry/geometries/geometries.hpp>


unsigned int PUnitLayer::AddUnit(PUnit * unit)
{
	unsigned int index = static_cast<unsigned int>(units.size());
	units.push_back(unit);
	blaze::StaticVector<double, 3> unit_pos = unit->GetPosition();
	return index;
}

void PUnitLayer::RemoveUnit(unsigned int unit_id)
{
	units[unit_id] = nullptr;
}

void PUnitLayer::UpdateUnit(unsigned int unit_id, blaze::StaticVector<double, 3> position)
{
}

std::vector<PUnit*> PUnitLayer::Nearby(unsigned int unit_id, double radius)
{
	PUnit* current = units.at(unit_id);
	std::vector<PUnit*> nearby;
	blaze::StaticVector<double, 3> unit_pos = current->GetPosition();



	return nearby;
}

PUnit * PUnitLayer::GetUnit(unsigned int unit_id)
{
	return units.at(unit_id);
}
