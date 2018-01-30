#include "stdafx.h"
#include "PUnitLayer.h"

void PUnitLayer::AddUnit(PUnit * unit, unsigned int unit_id)
{
	members[unit_id] = unit;
	list.push_back(unit);
}

void PUnitLayer::RemoveUnit(unsigned int unit_id)
{
	PUnit* unit = members.at(unit_id);
	for (auto it = list.begin(); it != list.end(); it++)
	{
		if ((*it) == unit) {
			list.erase(it);
			break;
		}
	}
	members.erase(unit_id);
}

void PUnitLayer::UpdateUnit(unsigned int unit_id)
{
	valid = false;
}

PUnit * PUnitLayer::GetUnit(unsigned int unit_id)
{
	return members.at(unit_id);
}

std::vector<PUnit*> PUnitLayer::Nearby(unsigned int unit_id, double radius)
{
	PUnit* current = members.at(unit_id);
	blaze::StaticVector<double, 3> center = current->GetPosition();

	if (!valid) {
		if (clusters != nullptr) {
			delete[] clusters;
		}

		std::vector<blaze::StaticVector<double, 3>> centroids;
		// build an array of units for faster access
		PUnit** unit_array = new PUnit*[list.size()];
		int unit_index = 0;
		for (auto it = list.begin(); it != list.end(); it++)
		{
			unit_array[unit_index] = *it;
			unit_index++;
		}
		// choose first centroid
		blaze::StaticVector<double, 3> first = list.front()->GetPosition();
		centroids.push_back(first);
		// randomly choose remaining centroids
		while (centroids.size() < k) {
			std::random_device rd;
			std::mt19937 gen(rd());
			std::uniform_real_distribution<> dis(0.0, 1.0);

			double total_weight = 0;
			double* weight_array = new double[list.size()];
			// calculate the weight of each unit
			for (size_t i = 0; i < list.size(); i++)
			{
				double lowest_weight = std::numeric_limits<double>::max();
				PUnit* p = unit_array[i];
				for (size_t j = 0; j < centroids.size(); j++)
				{
					double weight = blaze::sqrLength(p->GetPosition() - centroids.at(j));
					if (weight < lowest_weight) lowest_weight = weight;
				}
				weight_array[i] = lowest_weight;
				total_weight += lowest_weight;
			}
			// randomly select a unit based on weight
			double random_weight = dis(gen) * total_weight;
			double cumulative_weight = weight_array[0];
			unsigned int selected = 0;
			while (cumulative_weight < random_weight) {
				cumulative_weight += weight_array[++selected];
			}
			centroids.push_back(unit_array[selected]->GetPosition());
			delete[] weight_array;
		}
		// execute k-means iterations
		clusters = new std::vector<PUnit*>[centroids.size()];
		for (int i = 0; i < iterations; i++)
		{
			unsigned int* closest_centroids = new unsigned int[list.size()];
			for (size_t j = 0; j < list.size(); j++)
			{
				unsigned int closest = 0;
				double closest_distance = std::numeric_limits<double>::max();
				for (size_t k = 0; k < centroids.size(); k++)
				{
					double distance = blaze::sqrLength(unit_array[j]->GetPosition() - centroids.at(k));
					if (distance < closest_distance) {
						closest = k;
						closest_distance = distance;
					}
				}
				clusters[closest].push_back(unit_array[j]);
				cluster_allocation[unit_array[j]] = closest;
			}
			delete[] closest_centroids;
		}
		delete[] unit_array;
		valid = true;
	}

	return clusters[cluster_allocation.at(current)];
}


