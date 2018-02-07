#pragma once
#include "PUnit.h"
#include "PGraphNode.h"
#include "PGraph.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <list>
#include "blaze\Blaze.h"


class PUnitLayer
{
public:
	unsigned int id;
	void AddUnit(PUnit*, unsigned int unit_id);
	void RemoveUnit(unsigned int unit_id);
	void UpdateUnit(unsigned int unit_id);
	PUnit* GetUnit(unsigned int unit_id);
	std::unordered_set<unsigned int> Nearby(unsigned int unit_id);
	void SetGraph(PGraph* graph);
	PGraph* GetGraph();
private:
	PGraph* graph = nullptr;
	std::unordered_map<unsigned int, PUnit*> members;
	std::unordered_map<blaze::StaticVector<int, 3>, std::unordered_set<unsigned int>, PGraphNode::IndexHash> node_contents;
	std::unordered_map<unsigned int, blaze::StaticVector<int, 3>> node_allocation;
};

