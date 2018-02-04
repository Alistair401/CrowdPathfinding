#include "stdafx.h"
#include "PSystem.h"

void PSystem::InitGraph(blaze::StaticVector<double, 3> origin, blaze::StaticVector<double, 3> dimensions, double scale) {
	if (graph != nullptr) {
		delete graph;
	}
	graph = new PGraph(origin, dimensions, 16);
}

unsigned int PSystem::CreateUnit(blaze::StaticVector<double, 3> position, unsigned int layer_id)
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
	if (layers.find(layer_id) == layers.end()) {
		layers[layer_id] = new PUnitLayer();
	}
	layer_allocation[id] = layer_id;
	layers.at(layer_id)->AddUnit(unit, id);
	return id;
}

void PSystem::UpdateUnitTarget(unsigned int id, blaze::StaticVector<double, 3> target)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	layer->GetUnit(id)->UpdateTarget(target);
}

void PSystem::UpdateUnitPosition(unsigned int id, blaze::StaticVector<double, 3> position)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	layer->GetUnit(id)->UpdatePosition(position);
	layer->InvalidateClusters();
}

void PSystem::UpdateUnitHeading(unsigned int id, blaze::StaticVector<double, 3> heading)
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

blaze::StaticVector<double, 3> PSystem::GetUnitForce(unsigned int id)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	PUnit* current = layer->GetUnit(id);
	std::vector<PUnit*> nearby = layer->Nearby(id, 30);
	blaze::StaticVector<double, 3> separation_vector{ 0,0,0 };
	blaze::StaticVector<double, 3> alignment_vector{ 0,0,0 };
	blaze::StaticVector<double, 3> cohesion_vector{ 0,0,0 };
	blaze::StaticVector<double, 3> following_vector{ 0,0,0 };
	blaze::StaticVector<double, 3> target_vector{ 0,0,0 };
	blaze::StaticVector<double, 3> resultant_vector{ 0,0,0 };

	if (current->GetLeader() == 0) {
		target_vector = current->GetTarget() - current->GetPosition();
	}
	else {
		PUnit* leader = GetUnit(current->GetLeader());
		following_vector = leader->GetPosition() - current->GetPosition();
	}
	if (nearby.size() > 0) {
		for (size_t i = 0; i < nearby.size(); i++)
		{
			PUnit* u = nearby.at(i);
			if (u == current) continue;
			blaze::StaticVector<double, 3> separating_vector = u->GetPosition() - current->GetPosition();
			double separating_distance = blaze::sqrLength(separating_vector);
			separation_vector = separation_vector + (separating_vector / separating_distance);
			alignment_vector = alignment_vector + u->GetHeading();
			cohesion_vector = cohesion_vector + u->GetPosition();
		}

		separation_vector = separation_vector * -1;
		cohesion_vector = (cohesion_vector / static_cast<int>(nearby.size())) - current->GetPosition();
		alignment_vector = alignment_vector / static_cast<int>(nearby.size());
	}
	separation_vector = separation_vector * separation_factor;
	cohesion_vector = cohesion_vector * cohesion_factor;
	alignment_vector = alignment_vector * alignment_factor;
	following_vector = following_vector * following_factor;
	target_vector = target_vector * target_factor;

	resultant_vector = separation_vector + cohesion_vector + alignment_vector + following_vector + target_vector;
	return resultant_vector;
}

PUnit * PSystem::GetUnit(unsigned int unit_id)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(unit_id));
	PUnit* unit = layer->GetUnit(unit_id);
	return unit;
}
