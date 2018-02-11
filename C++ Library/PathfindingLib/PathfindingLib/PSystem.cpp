#include "stdafx.h"
#include "PSystem.h"
#include "Pathfinding.h"

void PSystem::InitGraph(unsigned int layer_id, blaze::StaticVector<float, 3>& origin, blaze::StaticVector<float, 3>& dimensions, float scale) {
	PGraph* graph = new PGraph(origin, dimensions, scale);
	layers.at(layer_id)->SetGraph(graph);
}

PGraph* PSystem::GetGraph(unsigned int layer_id)
{
	PUnitLayer* l = layers.at(layer_id);
	return l->GetGraph();
}

unsigned int PSystem::CreateUnit(blaze::StaticVector<float, 3>& position, unsigned int layer_id)
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

void PSystem::UpdateUnitTarget(unsigned int id, blaze::StaticVector<float, 3>& target)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	layer->GetUnit(id)->UpdateTarget(target);
}

void PSystem::UpdateUnitPosition(unsigned int id, blaze::StaticVector<float, 3>& position)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	layer->GetUnit(id)->UpdatePosition(position);
	layer->UpdateUnit(id);
}

void PSystem::UpdateUnitHeading(unsigned int id, blaze::StaticVector<float, 3>& heading)
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
	std::unordered_set<unsigned int> &nearby = layer->Nearby(id);
	blaze::StaticVector<float, 3> separation_vector{ 0,0,0 };
	blaze::StaticVector<float, 3> cohesion_vector{ 0,0,0 };
	blaze::StaticVector<float, 3> target_vector{0,0,0};


	current->SetLeader(0);

	// Evaluate nearby units for flocking behaviours
	if (nearby.size() > 0) {
		for (auto it = nearby.begin(); it != nearby.end(); it++)
		{
			unsigned int nearby_id = (*it);
			PUnit* u = layer->GetUnit(nearby_id);
			if (u == current) continue;

			blaze::StaticVector<float, 3> const& separating_vector = u->GetPosition() - current->GetPosition();

			float sqr_separating_distance = blaze::sqrLength(separating_vector);
			float sqr_nearby_target_distance = blaze::sqrLength(u->GetPosition() - u->GetTarget());
			float sqr_target_similarity = blaze::sqrLength(current->GetTarget() - u->GetTarget());
			float sqr_target_distance = blaze::sqrLength(current->GetTarget() - current->GetPosition());
			if (sqr_separating_distance < leader_distance_threshold // Unit is close enough to consider as a leader
				&& sqr_nearby_target_distance < sqr_target_distance // Unit is closer than this one to its target
				&& sqr_target_similarity < target_similarity_threshold // Unit has a similar enough target
				) current->SetLeader(nearby_id);

			separation_vector = separation_vector + (separating_vector / sqr_separating_distance);
			cohesion_vector = cohesion_vector + u->GetPosition();
		}
		separation_vector = separation_vector * -1.0f;
		cohesion_vector = (cohesion_vector / static_cast<float>(nearby.size())) - current->GetPosition();
	}
	else {
		current->SetLeader(0);
	}

	unsigned int leader_id = current->GetLeader();
	if (leader_id == 0) {
		// Calculate and save a path to the target
		std::vector<blaze::StaticVector<float, 3>>* path = layer->GetPath(id);
		if (path == nullptr) {
			path = Pathfinding::a_star(layer->GetGraph(), current->GetPosition(), current->GetTarget());
			layer->SetPath(id, path);
		}
		blaze::StaticVector<float, 3>& next = path->back();
		float sqr_next_distance = blaze::sqrLength(current->GetPosition() - next);
		while (sqr_next_distance < 36 && path->size() > 1) {
			path->pop_back();
			next = path->back();
			sqr_next_distance = blaze::sqrLength(current->GetPosition() - next);
		}
		target_vector = next - current->GetPosition();
		target_vector = target_vector / std::sqrt(sqr_next_distance);
	}
	else {
		PUnit* leader = GetUnit(leader_id);
		target_vector = leader->GetPosition() - current->GetPosition();
		target_vector = target_vector / blaze::length(leader->GetPosition() - current->GetPosition());
	}

	separation_vector = separation_vector * separation_factor;
	cohesion_vector = cohesion_vector * cohesion_factor;
	target_vector = target_vector * target_factor;

	blaze::StaticVector<float, 3> const& resultant_vector = separation_vector + cohesion_vector + target_vector;
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
