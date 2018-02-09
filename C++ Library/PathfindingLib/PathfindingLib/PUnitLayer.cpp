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

PGraph * PUnitLayer::GetGraph()
{
	return graph;
}

std::unordered_set<unsigned int> PUnitLayer::Nearby(unsigned int unit_id)
{
	PUnit* current = members.at(unit_id);
	std::unordered_set<unsigned int> results;

	if (graph == nullptr) return results;

	std::unordered_set<unsigned int>& center_contents = node_contents.at(node_allocation.at(unit_id));

	results.insert(center_contents.begin(),center_contents.end());

	std::vector<PGraphNode*>& neighbors = graph->NodeAtIndex(node_allocation.at(unit_id))->neighbors;
	for (size_t i = 0; i < neighbors.size(); i++)
	{
		PGraphNode* n = neighbors.at(i);
		if (node_contents.find(n->index) == node_contents.end()) continue;
		std::unordered_set<unsigned int>& contents = node_contents.at(n->index);
		results.insert(contents.begin(), contents.end());
	}

	return results;
}


