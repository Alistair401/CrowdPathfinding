#pragma once
#include "Common.h"
#include "PGraph.h"
#include "PUnit.h"
#include "PUnitLayer.h"
#include "GL\glew.h"
#include <string>

class PSystem
{
public:
	// Singleton
	static PSystem& GetInstance();
	PSystem(PSystem const&) = delete;
	void operator=(PSystem const&) = delete;
	// Graph Management
	PGraph* InitGraph(unsigned int layer_id, Vector3& origin, Vector3& dimensions, float scale);
	// Unit Management
	unsigned int CreateUnit(Vector3& position, unsigned int layer_id);
	void UpdateUnitTarget(unsigned int id, Vector3& target);
	void UpdateUnitPosition(unsigned int id, Vector3& position);
	void DestroyUnit(unsigned int id);
	// Unit Interaction
	void UpdateInteractions();
	Vector3 GetUnitForce(unsigned int id);
	// Layer Management
	void CreateLayer(unsigned int layer_id);
	// Statistics
	std::string Stats();
	void ResetStats();
private:
	// Singleton
	PSystem();
	// Unit Management
	unsigned int next_unit_id = 1;
	std::vector<unsigned int> free_unit_ids;
	PUnit* GetUnit(unsigned int unit_id);
	// Unit Interaction
	std::unordered_map<unsigned int, Vector3> forces;
	float follow_factor = 0.004f;
	float target_similarity_threshold = 50.0f;
	float avoidance_factor = 6.0f;
	float lookahead = 20.0f;
	// Layer Management
	std::unordered_map<unsigned int, PUnitLayer*> layers;
	std::unordered_map<unsigned int, unsigned int> layer_allocation;
	// OpenGL
	GLuint unit_ssbo_binding = 1;
	GLuint unit_ssbo;
	GLuint count_ssbo_binding = 2;
	GLuint count_ssbo;
	GLuint index_ssbo_binding = 3;
	GLuint index_ssbo;
	GLuint neighbor_ssbo_binding = 4;
	GLuint neighbor_ssbo;
	GLuint output_ssbo_binding = 5;
	GLuint output_ssbo;
};
