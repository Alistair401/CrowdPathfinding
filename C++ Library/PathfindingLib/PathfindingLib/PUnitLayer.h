#pragma once
#include "Common.h"
#include "PUnit.h"
#include "PGraphNode.h"
#include "PGraph.h"
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <list>


class PUnitLayer
{
public:
	unsigned int id;

	void AddUnit(PUnit*, unsigned int unit_id);
	void RemoveUnit(unsigned int unit_id);
	void UpdateUnit(unsigned int unit_id);
	PUnit* GetUnit(unsigned int unit_id);

	void SetPath(unsigned int unit_id, std::vector<Vector3>* path);
	std::vector<Vector3>* GetPath(unsigned int unit_id);
	void ClearPath(unsigned int unit_id);

	void SetGraph(PGraph* graph);
	PGraph* GetGraph();

	std::vector<std::unordered_set<unsigned int>*> Nearby(unsigned int unit_id);
private:
	PGraph * graph = nullptr;
	std::unordered_map<unsigned int, PUnit*> members;
	std::unordered_map<IVector3, std::unordered_set<unsigned int>*, IVector3Hash> node_contents;
	std::unordered_map<unsigned int, IVector3> node_allocation;
	std::unordered_map<unsigned int, std::vector<Vector3>*> path_allocation;
};
