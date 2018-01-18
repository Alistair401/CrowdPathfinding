#include "stdafx.h"
#include "UnitMediator.h"

unsigned int UnitMediator::AddUnit(PUnit * unit, unsigned int layer_id)
{
	// -- temporary --
	if (temp_leader == nullptr) {
		temp_leader = unit;
	}
	else {
		unit->SetLeader(temp_leader);
	}
	// ---------------
	if (layers.find(layer_id) == layers.end()) {
		layers[layer_id] = new PUnitLayer();
	}
	return layers.at(layer_id)->AddUnit(unit);
}

blaze::StaticVector<double, 3> UnitMediator::GetForce(unsigned int layer_id, unsigned int unit_id)
{
	PUnitLayer* l = layers.at(layer_id);
	PUnit* current = l->GetUnit(unit_id);
	std::vector<PUnit*> nearby = l->Nearby(unit_id, 80);
	blaze::StaticVector<double, 3> separation_vector{ 0,0,0 };
	blaze::StaticVector<double, 3> alignment_vector{ 0,0,0 };
	blaze::StaticVector<double, 3> cohesion_vector{ 0,0,0 };
	blaze::StaticVector<double, 3> following_vector{ 0,0,0 };
	blaze::StaticVector<double, 3> resultant_vector{ 0,0,0 };

	if (current->GetLeader() != nullptr) {
		following_vector = current->GetLeader()->GetPosition() - current->GetPosition();
	}

	if (nearby.size() > 0) {
		for (size_t i = 0; i < nearby.size(); i++)
		{
			PUnit* u = nearby.at(i);
			blaze::StaticVector<double, 3> separating_vector = u->GetPosition() - current->GetPosition();
			double separating_distance = blaze::length(separating_vector);
			separation_vector = separation_vector + (separating_vector / std::pow(separating_distance,2));
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

	resultant_vector = separation_vector + cohesion_vector + alignment_vector + following_vector;
	return resultant_vector;
}