#pragma once
#include "blaze\Blaze.h"
#include "PGraph.h"
#include "PUnit.h"
#include "PUnitLayer.h"

class PSystem
{
public:
	// Singleton
	static PSystem& GetInstance() {
		static PSystem instance;
		return instance;
	}
	PSystem(PSystem const&) = delete;
	void operator=(PSystem const&) = delete;
	// Graph Management
	void InitGraph(unsigned int layer_id, blaze::StaticVector<double, 3> origin, blaze::StaticVector<double, 3> dimensions, double scale);
	// Unit Management
	unsigned int CreateUnit(blaze::StaticVector<double, 3> position, unsigned int layer_id);
	void UpdateUnitTarget(unsigned int id, blaze::StaticVector<double, 3> target);
	void UpdateUnitPosition(unsigned int id, blaze::StaticVector<double, 3> position);
	void UpdateUnitHeading(unsigned int id, blaze::StaticVector<double, 3> heading);
	void DestroyUnit(unsigned int id);
	// Unit Interaction
	blaze::StaticVector<double, 3> GetUnitForce(unsigned int id);
	// Layer Management
	void CreateLayer(unsigned int layer_id);
private:
	// Singleton
	PSystem() {};
	// Unit Management
	unsigned int next_unit_id = 1;
	std::vector<unsigned int> free_unit_ids;
	PUnit* GetUnit(unsigned int unit_id);
	// Unit Interaction
	double cohesion_factor = 0.001;
	double separation_factor = 1;
	double alignment_factor = 0 /*0.1*/;
	double following_factor = 0.01;
	double target_factor = 0.01;
	// Layer Management
	std::unordered_map<unsigned int, PUnitLayer*> layers;
	std::unordered_map<unsigned int, unsigned int> layer_allocation;
};
