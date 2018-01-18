#include "stdafx.h"
#include "Pathfinding.h"
#include "NodeMinHeap.h"
#include <unordered_map>
#include <unordered_set>
#include <limits>

double EuclideanDistance(blaze::StaticVector<double, 3> a, blaze::StaticVector<double, 3> b) {
	blaze::StaticVector<double, 3> direction = b - a;
	return blaze::length(direction);
}

double HeuristicCostEstimate(PGraphNode* a, PGraphNode* b) {
	return EuclideanDistance(a->position, b->position);
}

std::vector<blaze::StaticVector<double, 3>>* reconstruct_path(std::unordered_map<blaze::StaticVector<int, 3>, PGraphNode*, PGraphNode::IndexHash>* came_from, PGraphNode* final_node) {
	std::vector<blaze::StaticVector<double, 3>>* result = new std::vector<blaze::StaticVector<double, 3>>;
	PGraphNode* current = final_node;
	result->push_back(current->position);
	while (came_from->find(current->index) != came_from->end()) {
		current = came_from->at(current->index);
		result->push_back(current->position);
	}
	std::reverse(result->begin(), result->end());
	return result;
}

std::vector<blaze::StaticVector<double, 3>>* Pathfinding::a_star(PGraph * graph, blaze::StaticVector<double, 3> from, blaze::StaticVector<double, 3> to)
{
	PGraphNode* from_node = graph->NodeAt(from);
	PGraphNode* to_node = graph->NodeAt(to);

	std::unordered_map<blaze::StaticVector<int, 3>, float, PGraphNode::IndexHash> f_score;
	std::unordered_set<blaze::StaticVector<int, 3>, PGraphNode::IndexHash> closed_set;
	std::unordered_map<blaze::StaticVector<int, 3>, PGraphNode*, PGraphNode::IndexHash> came_from;
	std::unordered_map<blaze::StaticVector<int, 3>, float, PGraphNode::IndexHash> g_score;

	NodeMinHeap open_set(&f_score);
	f_score.emplace(from_node->index, HeuristicCostEstimate(from_node, to_node)); // must insert into f_score before open_set
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

			float tentative_g_score = g_score.at(current->index) + static_cast<float>(EuclideanDistance(current->position, neighbor->position));
			float current_g_score = FLT_MAX;
			if (g_score.find(neighbor->index) != g_score.end()) {
				current_g_score = g_score.at(neighbor->index);
			}
			if (tentative_g_score >= current_g_score) {
				continue;
			}

			came_from[neighbor->index] = current;
			g_score[neighbor->index] = tentative_g_score;
			f_score[neighbor->index] = g_score.at(neighbor->index) + static_cast<float>(HeuristicCostEstimate(neighbor, to_node));

			if (open_set.Contains(neighbor)) {
				open_set.Update(neighbor);
			}
			else {
				open_set.Insert(neighbor);
			}
		}
	}

	return new std::vector<blaze::StaticVector<double, 3>>;
}
