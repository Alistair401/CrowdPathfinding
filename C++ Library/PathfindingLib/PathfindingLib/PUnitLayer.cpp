#include "stdafx.h"
#include "PUnitLayer.h"

void PUnitLayer::AddUnit(PUnit * unit, unsigned int unit_id)
{
	members[unit_id] = unit;
	UpdateUnit(unit_id);
}

void PUnitLayer::RemoveUnit(unsigned int unit_id)
{
	members.erase(unit_id);
	if (node_allocation.find(unit_id) != node_allocation.end()) {
		blaze::StaticVector<int, 3> old_index = node_allocation.at(unit_id);
		node_contents.at(old_index).erase(unit_id);
		node_allocation.erase(unit_id);
	}
}

void PUnitLayer::UpdateUnit(unsigned int unit_id)
{
	PUnit* unit = members.at(unit_id);
	blaze::StaticVector<int, 3> node_index = graph->NodeAt(unit->GetPosition())->index;
	// If the unit has a previous allocation, remove it
	if (node_allocation.find(unit_id) != node_allocation.end()) {
		blaze::StaticVector<int, 3> old_index = node_allocation.at(unit_id);
		node_contents.at(old_index).erase(unit_id);
	}
	// Create a new set if a node hasn't been seen before
	if (node_contents.find(node_index) == node_contents.end()) {
		node_contents[node_index] = std::unordered_set<unsigned int>();
	}
	// Add the unit to the node and allocate that node to the unit
	node_contents.at(node_index).insert(unit_id);
	node_allocation[unit_id] = node_index;
}

PUnit * PUnitLayer::GetUnit(unsigned int unit_id)
{
	return members.at(unit_id);
}

void PUnitLayer::SetGraph(PGraph * graph)
{
	this->graph = graph;
}

std::vector<PUnit*> PUnitLayer::Nearby(unsigned int unit_id, float radius)
{
	PUnit* current = members.at(unit_id);

	if (graph == nullptr) return std::vector<PUnit*>();

	std::vector<PUnit*> nearby;
	std::unordered_set<unsigned int> contents = node_contents.at(node_allocation.at(unit_id));
	for (auto it = contents.begin(); it != contents.end(); it++)
	{
		nearby.push_back(GetUnit(*it));
	}

	return nearby;
}


