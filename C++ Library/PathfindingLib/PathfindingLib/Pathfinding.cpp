#include "stdafx.h"
#include "Pathfinding.h"
#include <queue>
#include <set>

double heuristic_cost_estimate(PGraphNode* a, PGraphNode* b) {
	return 0;
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
	std::priority_queue<PGraphNode*, std::vector<PGraphNode*>, decltype(compare_f_score)> open_set(compare_f_score);

	auto compare_node = [](PGraphNode* a, PGraphNode* b) {return a->index == b->index; };
	std::set<PGraphNode*,decltype(compare_node)> closed_set(compare_node);

	open_set.push(start_node);
	g_score.insert(std::make_pair(start_node,0.0));
	f_score.insert(std::make_pair(start_node, heuristic_cost_estimate(start_node,end_node)));

	return new blaze::DynamicVector<blaze::StaticVector<double, 3UL>, blaze::rowVector>();
}

