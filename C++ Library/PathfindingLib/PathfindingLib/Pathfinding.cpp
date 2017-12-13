#include "stdafx.h"
#include "Pathfinding.h"
#include <set>
#include <queue>

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

blaze::DynamicVector<blaze::StaticVector<double, 3UL>, blaze::rowVector>* Pathfinding::a_star(PGraph* graph, blaze::StaticVector<double, 3UL> from, blaze::StaticVector<double, 3UL> to)
{
	PGraphNode* start_node = graph->get_node_at(from);
	PGraphNode* end_node = graph->get_node_at(to);

	if (*start_node == *end_node) {
		return new blaze::DynamicVector<blaze::StaticVector<double, 3UL>, blaze::rowVector>();
	}

	std::unordered_map<PGraphNode*, double> f_score;
	std::unordered_map<PGraphNode*, double> g_score;
	std::unordered_map<PGraphNode*, PGraphNode*> came_from = {};


	auto compare_f_score = [&f_score](PGraphNode* a, PGraphNode* b) { return f_score.at(a) < f_score.at(b); };
	std::priority_queue<PGraphNode*, std::vector<PGraphNode*>, decltype(compare_f_score)> open_set(compare_f_score); // TODO: replace with proper heap implementation
	

	auto compare_node = [](PGraphNode* a, PGraphNode* b) {return a->index == b->index; };
	std::set<PGraphNode*, decltype(compare_node)> closed_set(compare_node);
	std::set<PGraphNode*, decltype(compare_node)> aux_open_set(compare_node); // TODO: replace with proper heap implementation

	open_set.push(start_node);
	aux_open_set.insert(start_node);

	g_score.insert(std::make_pair(start_node, 0.0));
	f_score.insert(std::make_pair(start_node, heuristic_cost_estimate(start_node, end_node)));

	while (open_set.size() > 0) {
		PGraphNode* current_node = open_set.top();
		if (&current_node == &end_node) {
			// TODO: reconstruct path
		}
		open_set.pop();
		aux_open_set.erase(current_node);
		closed_set.insert(current_node);

		for (int i = 0; i < current_node->neighbors.size(); i++)
		{
			if (closed_set.find(current_node->neighbors[i]) != closed_set.end()) {
				continue;
			}
			if (aux_open_set.find(current_node->neighbors[i]) == aux_open_set.end()) {
				open_set.push(current_node->neighbors[i]);
				aux_open_set.insert(current_node->neighbors[i]);
			}
			double tentative_g_score = g_score.at(current_node) + euclidean_distance(current_node->position, current_node->neighbors[i]->position);
			//TODO: update neighbor
		}
	}

	return new blaze::DynamicVector<blaze::StaticVector<double, 3UL>, blaze::rowVector>();
}
