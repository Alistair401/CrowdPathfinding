#include "stdafx.h"
#include "PSystem.h"

void PSystem::InitGraph(unsigned int layer_id, blaze::StaticVector<float, 3> origin, blaze::StaticVector<float, 3> dimensions, float scale) {
	PGraph* graph = new PGraph(origin, dimensions, scale);
	layers.at(layer_id)->SetGraph(graph);
}

PGraph* PSystem::GetGraph(unsigned int layer_id)
{
	PUnitLayer* l = layers.at(layer_id);
	return l->GetGraph();
}

unsigned int PSystem::CreateUnit(blaze::StaticVector<float, 3> position, unsigned int layer_id)
{
	// Create the unit
	PUnit* unit = new PUnit(position);
	// Assign it an ID
	unsigned int id;
	if (free_unit_ids.size() > 0) {
		id = free_unit_ids.back();
		free_unit_ids.pop_back();
	}
	else {
		id = next_unit_id++;
	}
	// Assign it to a layer
	layer_allocation[id] = layer_id;
	layers.at(layer_id)->AddUnit(unit, id);
	return id;
}

void PSystem::UpdateUnitTarget(unsigned int id, blaze::StaticVector<float, 3> target)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	layer->GetUnit(id)->UpdateTarget(target);
}

void PSystem::UpdateUnitPosition(unsigned int id, blaze::StaticVector<float, 3> position)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	layer->GetUnit(id)->UpdatePosition(position);
	layer->UpdateUnit(id);
}

void PSystem::UpdateUnitHeading(unsigned int id, blaze::StaticVector<float, 3> heading)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	layer->GetUnit(id)->UpdateHeading(heading);
}

void PSystem::DestroyUnit(unsigned int id)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	PUnit* unit = layer->GetUnit(id);
	layer->RemoveUnit(id);
	layer_allocation.erase(id);
	delete unit;
}

blaze::StaticVector<float, 3> PSystem::GetUnitForce(unsigned int id)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	PUnit* current = layer->GetUnit(id);
	std::unordered_set<unsigned int> nearby = layer->Nearby(id);
	blaze::StaticVector<float, 3> separation_vector{ 0,0,0 };
	blaze::StaticVector<float, 3> cohesion_vector{ 0,0,0 };
	blaze::StaticVector<float, 3> following_vector{ 0,0,0 };
	blaze::StaticVector<float, 3> target_vector{ 0,0,0 };
	blaze::StaticVector<float, 3> resultant_vector{ 0,0,0 };

	if (current->GetLeader() == 0) {
		target_vector = current->GetTarget() - current->GetPosition();
	}
	else {
		PUnit* leader = GetUnit(current->GetLeader());
		following_vector = leader->GetPosition() - current->GetPosition();
	}
	if (nearby.size() > 0) {
		int actual_neighbours = 0;
		for (auto it = nearby.begin(); it != nearby.end(); it++)
		{
			PUnit* u = layer->GetUnit(*it);
			if (u == current) continue;
			blaze::StaticVector<float, 3> separating_vector = u->GetPosition() - current->GetPosition();
			float separating_distance = blaze::sqrLength(separating_vector);
			separation_vector = separation_vector + (separating_vector / separating_distance);
			cohesion_vector = cohesion_vector + u->GetPosition();
			actual_neighbours++;
		}
		if (actual_neighbours > 0) {
			separation_vector = separation_vector * -1;
			cohesion_vector = (cohesion_vector / static_cast<int>(nearby.size())) - current->GetPosition();
		}
	}
	separation_vector = separation_vector * separation_factor;
	cohesion_vector = cohesion_vector * cohesion_factor;
	following_vector = following_vector * following_factor;
	target_vector = target_vector * target_factor;

	resultant_vector = separation_vector + cohesion_vector + following_vector + target_vector;
	return resultant_vector;
}

void PSystem::CreateLayer(unsigned int layer_id)
{
	if (layers.find(layer_id) == layers.end()) {
		layers[layer_id] = new PUnitLayer();
	}
}

PUnit * PSystem::GetUnit(unsigned int unit_id)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(unit_id));
	PUnit* unit = layer->GetUnit(unit_id);
	return unit;
}
