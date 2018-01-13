#include "stdafx.h"
#include "Pathfinding.h"
#include "NodeMinHeap.h"
#include <unordered_map>
#include <unordered_set>
#include <limits>

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

std::vector<blaze::StaticVector<double, 3UL>>* reconstruct_path(std::unordered_map<blaze::StaticVector<int, 3UL>, PGraphNode*, PGraphNode::IndexHash>* came_from, PGraphNode* final_node) {
	std::vector<blaze::StaticVector<double, 3UL>>* result = new std::vector<blaze::StaticVector<double, 3UL>>;
	PGraphNode* current = final_node;
	result->push_back(current->position);
	while (came_from->find(current->index) != came_from->end()) {
		current = came_from->at(current->index);
		result->push_back(current->position);
	}
	std::reverse(result->begin(), result->end());
	return result;
}

std::vector<blaze::StaticVector<double, 3UL>>* Pathfinding::a_star(PGraph * graph, blaze::StaticVector<double, 3UL> from, blaze::StaticVector<double, 3UL> to)
{
	PGraphNode* from_node = graph->get_node_at(from);
	PGraphNode* to_node = graph->get_node_at(to);

	std::unordered_map<blaze::StaticVector<int, 3UL>, float, PGraphNode::IndexHash> f_score;
	std::unordered_set<blaze::StaticVector<int, 3UL>, PGraphNode::IndexHash> closed_set;
	std::unordered_map<blaze::StaticVector<int, 3UL>, PGraphNode*, PGraphNode::IndexHash> came_from;
	std::unordered_map<blaze::StaticVector<int, 3UL>, float, PGraphNode::IndexHash> g_score;

	NodeMinHeap open_set(&f_score);
	f_score.emplace(from_node->index, heuristic_cost_estimate(from_node, to_node)); // must insert into f_score before open_set
	g_score.emplace(from_node->index, 0);
	open_set.Insert(from_node);

	while (!open_set.IsEmpty()) {
		PGraphNode* current = open_set.Remove();
		if (current == to_node) {
			return reconstruct_path(&came_from, to_node);
		}
		closed_set.emplace(current->index);

		for (size_t i = 0; i < current->neighbors.size(); i++)
		{
			PGraphNode* neighbor = current->neighbors.at(i);
			if (closed_set.find(neighbor->index) != closed_set.end()) {
				continue;
			}

			float tentative_g_score = g_score.at(current->index) + euclidean_distance(current->position, neighbor->position);
			if (g_score.find(neighbor->index) != g_score.end() && tentative_g_score >= g_score.at(neighbor->index)) {
				continue;
			}

			came_from.emplace(neighbor->index, current);
			g_score.emplace(neighbor->index, tentative_g_score);
			f_score.emplace(neighbor->index, g_score.at(neighbor->index) + heuristic_cost_estimate(neighbor, to_node));

			if (!open_set.Contains(neighbor)) {
				open_set.Insert(neighbor);
			}
		}
	}

	return new std::vector<blaze::StaticVector<double, 3UL>>;
}
