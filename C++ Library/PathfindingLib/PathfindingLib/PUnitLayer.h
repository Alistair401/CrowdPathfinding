#pragma once
#include "PUnit.h"
#include <vector>
#include <unordered_map>
#include <list>
#include "blaze\Blaze.h"

class PUnitLayer
{
public:
	unsigned int id;
	void AddUnit(PUnit*, unsigned int unit_id);
	void RemoveUnit(unsigned int unit_id);
	void UpdateUnit(unsigned int unit_id);
	void InvalidateClusters();
	std::vector<PUnit*> Nearby(unsigned int unit_id, double radius);
	PUnit* GetUnit(unsigned int unit_id);
private:
	int k = 2;
	int iterations = 2;
	bool valid = false;
	std::unordered_map<unsigned int, PUnit*> members;
	std::list<PUnit*> list;
	std::unordered_map<PUnit*, unsigned int> cluster_allocation;
	std::vector<PUnit*>* clusters = nullptr;
};

