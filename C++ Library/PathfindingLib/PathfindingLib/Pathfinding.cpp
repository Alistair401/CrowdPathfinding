#include "stdafx.h"
#include "Pathfinding.h"
#include "NodeMinHeap.h"
#include <unordered_map>
#include <unordered_set>
#include <limits>

unsigned int Pathfinding::nodes_evaluated = 0;
unsigned int Pathfinding::a_star_calls = 0;

float EuclideanDistance(Vector3& a, Vector3& b) {
	Vector3 direction = b - a;
	return glm::length(direction);
}

float HeuristicCostEstimate(PGraphNode* a, PGraphNode* b) {
	return EuclideanDistance(a->position, b->position);
}

std::vector<Vector3>* reconstruct_path(std::unordered_map<IVector3, PGraphNode*, IVector3Hash>* came_from, PGraphNode* final_node) {
	std::vector<Vector3>* result = new std::vector<Vector3>;
	PGraphNode* current = final_node;
	result->push_back(current->position);
	while (came_from->find(current->index) != came_from->end()) {
		current = came_from->at(current->index);
		result->push_back(current->position);
	}
	return result;
}

std::vector<Vector3>* Pathfinding::a_star(PGraph * graph, Vector3 from, Vector3 to)
{
	a_star_calls++;
	PGraphNode* from_node = graph->NodeAt(from);
	PGraphNode* to_node = graph->NodeAt(to);

	std::unordered_map<IVector3, float, IVector3Hash> f_score;
	std::unordered_set<IVector3, IVector3Hash> closed_set;
	std::unordered_map<IVector3, PGraphNode*, IVector3Hash> came_from;
	std::unordered_map<IVector3, float, IVector3Hash> g_score;

	NodeMinHeap open_set(&f_score);
	f_score.emplace(from_node->index, HeuristicCostEstimate(from_node, to_node)); // must insert into f_score before open_set
	g_score.emplace(from_node->index, 0);
	open_set.Insert(from_node);

	while (!open_set.IsEmpty()) {
		nodes_evaluated++;
		PGraphNode* current = open_set.Remove();
		if (current == to_node) {
			return reconstruct_path(&came_from, to_node);
		}
		closed_set.emplace(current->index);

		for (size_t i = 0; i < current->neighbors.size(); i++)
		{
			PGraphNode* neighbor = current->neighbors.at(i);
			if (neighbor->obstacle) continue;
			if (closed_set.find(neighbor->index) != closed_set.end()) {
				continue;
			}

			float tentative_g_score = g_score.at(current->index) + EuclideanDistance(current->position, neighbor->position);
			float current_g_score = FLT_MAX;
			if (g_score.find(neighbor->index) != g_score.end()) {
				current_g_score = g_score.at(neighbor->index);
			}
			if (tentative_g_score >= current_g_score) {
				continue;
			}

			came_from[neighbor->index] = current;
			g_score[neighbor->index] = tentative_g_score;
			f_score[neighbor->index] = g_score.at(neighbor->index) + HeuristicCostEstimate(neighbor, to_node);

			if (open_set.Contains(neighbor)) {
				open_set.Update(neighbor);
			}
			else {
				open_set.Insert(neighbor);
			}
		}
	}

	return new std::vector<Vector3>;
}

void Pathfinding::ResetStats()
{
	nodes_evaluated = 0;
	a_star_calls = 0;
}

unsigned int Pathfinding::EvaluatedCount()
{
	return nodes_evaluated;
}

unsigned int Pathfinding::CallCount()
{
	return a_star_calls;
}
