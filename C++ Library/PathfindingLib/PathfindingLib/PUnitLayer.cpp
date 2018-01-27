#include "stdafx.h"
#include "PUnitLayer.h"
#include <boost/geometry/geometries/box.hpp>

typedef bg::model::box<point_t> box_t;

void PUnitLayer::AddUnit(PUnit * unit, unsigned int unit_id)
{
	blaze::StaticVector<double, 3> unit_pos = unit->GetPosition();
	point_t point(unit_pos[0], unit_pos[1], unit_pos[2]);
	value_t val = std::make_pair(point, unit);
	members[unit_id] = val;
	rtree.insert(val);
}

void PUnitLayer::RemoveUnit(unsigned int unit_id)
{
	rtree.remove(members[unit_id]);
	members.erase(unit_id);
}

void PUnitLayer::UpdateUnit(unsigned int unit_id)
{
	rtree.remove(members[unit_id]);
	PUnit* unit = members[unit_id].second;
	blaze::StaticVector<double, 3> unit_pos = unit->GetPosition();
	point_t point(unit_pos[0], unit_pos[1], unit_pos[2]);
	value_t val = std::make_pair(point, unit);
	members[unit_id] = val;
	rtree.insert(val);
}

PUnit * PUnitLayer::GetUnit(unsigned int unit_id)
{
	return members.at(unit_id).second;
}

std::vector<PUnit*> PUnitLayer::Nearby(unsigned int unit_id, double radius)
{
	PUnit* current = members.at(unit_id).second;
	std::vector<PUnit*> nearby;
	blaze::StaticVector<double, 3> center = current->GetPosition();

	point_t min_point(center[0] - radius, center[1] - radius, center[2] - radius);
	point_t max_point(center[0] + radius, center[1] + radius, center[2] + radius);
	box_t query_box(min_point, max_point);
	std::vector<value_t> query_results;

	rtree.query(bgi::intersects(query_box), std::back_inserter(query_results));

	for (size_t i = 0; i < query_results.size(); i++)
	{
		PUnit* neighbor = query_results.at(i).second;
		if (neighbor == current) continue;
		nearby.push_back(neighbor);
	}

	return nearby;
}


