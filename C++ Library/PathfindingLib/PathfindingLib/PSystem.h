#pragma once
#include "blaze\Blaze.h"
#include "PGraph.h"
#include "PUnit.h"
#include "PUnitLayer.h"
#include "GL\glew.h"

class PSystem
{
public:
	// Singleton
	static PSystem& GetInstance();
	PSystem(PSystem const&) = delete;
	void operator=(PSystem const&) = delete;
	// Graph Management
	PGraph* InitGraph(unsigned int layer_id, blaze::StaticVector<float, 3>& origin, blaze::StaticVector<float, 3>& dimensions, float scale);
	PGraph* GetGraph(unsigned int layer_id); // DEBUG
	// Unit Management
	unsigned int CreateUnit(blaze::StaticVector<float, 3>& position, unsigned int layer_id);
	void UpdateUnitTarget(unsigned int id, blaze::StaticVector<float, 3>& target);
	void UpdateUnitPosition(unsigned int id, blaze::StaticVector<float, 3>& position);
	void UpdateUnitHeading(unsigned int id, blaze::StaticVector<float, 3>& heading);
	void DestroyUnit(unsigned int id);
	// Unit Interaction
	void UpdateInteractions();
	blaze::StaticVector<float, 3> GetUnitForce(unsigned int id);
	// Layer Management
	void CreateLayer(unsigned int layer_id);
private:
	// Singleton
	PSystem();
	// Unit Management
	unsigned int next_unit_id = 1;
	std::vector<unsigned int> free_unit_ids;
	PUnit* GetUnit(unsigned int unit_id);
	// Unit Interaction
	std::unordered_map<unsigned int, blaze::StaticVector<float, 3>> forces;
	float cohesion_factor = 0.001f;
	float separation_factor = 1;
	float target_factor = 0.1f;
	float leader_distance_threshold = 50;
	float target_similarity_threshold = 50;
	// Layer Management
	std::unordered_map<unsigned int, PUnitLayer*> layers;
	std::unordered_map<unsigned int, unsigned int> layer_allocation;
	// OpenGL
	GLuint unit_ssbo_index;
	GLuint unit_ssbo;
	GLuint count_ssbo_index;
	GLuint count_ssbo;
	GLuint index_ssbo_index;
	GLuint index_ssbo;
	GLuint neighbor_ssbo_index;
	GLuint neighbor_ssbo;
	GLuint output_ssbo_index;
	GLuint output_ssbo;
};
