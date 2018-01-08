#include "stdafx.h"
#include "Pathfinding.h"
#include "NodeMinHeap.h"
#include <unordered_map>

double euclidean_distance(blaze::StaticVector<double, 3> a, blaze::StaticVector<double, 3> b) {
	blaze::StaticVector<double, 3> dist = a - b;
	blaze::StaticVector<double, 3> square = blaze::pow(dist, 2);
	double sum = 0;
	for (int i = 0; i < square.size(); i++)
	{
		sum += square[i];
	}
	if (sum == 0) {
		return 0;
	}
	double result = sqrt(sum);
	return result;
}

double heuristic_cost_estimate(PGraphNode* a, PGraphNode* b) {
	return euclidean_distance(a->position, b->position);
}



std::vector<blaze::StaticVector<double, 3UL>>* Pathfinding::a_star(PGraph * graph, blaze::StaticVector<double, 3UL> from, blaze::StaticVector<double, 3UL> to)
{
	std::unordered_map<blaze::StaticVector<int, 3UL>, float, PGraphNode::IndexHash> f_score;
	NodeMinHeap open_set(&f_score);
	PGraphNode* from_node = graph->get_node_at(from);
	open_set.Insert(from_node);
	return new std::vector<blaze::StaticVector<double, 3UL>>();
}
