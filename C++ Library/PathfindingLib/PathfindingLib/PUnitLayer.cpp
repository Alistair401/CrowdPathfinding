#include "stdafx.h"
#include "PUnitLayer.h"
#include <boost/geometry/geometries/ring.hpp>


unsigned int PUnitLayer::AddUnit(PUnit * unit)
{
	unsigned int index = static_cast<unsigned int>(units.size());
	units.push_back(unit);
	blaze::StaticVector<double,3> unit_pos = unit->GetPosition();
	point p = point(unit_pos[0], unit_pos[1], unit_pos[2]);
	rtree.insert(std::make_pair(p, unit));
	return index;
}

void PUnitLayer::RemoveUnit(unsigned int unit_id)
{
	units[unit_id] = nullptr;
}

void PUnitLayer::UpdateUnit(unsigned int unit_id,blaze::StaticVector<double,3> position)
{
	//update unit position in rtree
}

std::vector<PUnit*> PUnitLayer::Nearby(unsigned int unit_id, double radius)
{
	PUnit* current = units.at(unit_id);
	std::vector<PUnit*> result;
	blaze::StaticVector<double, 3> unit_pos = current->GetPosition();
	point p = point(unit_pos[0], unit_pos[1], unit_pos[2]);
	// query r tree using polygon or ring
	return result;
}

PUnit * PUnitLayer::GetUnit(unsigned int unit_id)
{
	return units.at(unit_id);
}
