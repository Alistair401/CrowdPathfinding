#include "stdafx.h"
#include "PSystem.h"
#include "Pathfinding.h"
#include "GLFW\glfw3.h"
#include "glm\glm.hpp"
#include <fstream>

glm::vec4 ToGLMVec4(Vector3& blaze_vec) {
	return glm::vec4(blaze_vec.at(0), blaze_vec.at(1), blaze_vec.at(2), 0);
}

PSystem& PSystem::GetInstance()
{
	static PSystem instance;

	return instance;
}

PGraph* PSystem::InitGraph(unsigned int layer_id, Vector3& origin, Vector3& dimensions, float scale) {
	PGraph* graph = new PGraph(origin, dimensions, scale);
	layers.at(layer_id)->SetGraph(graph);
	return graph;
}

unsigned int PSystem::CreateUnit(Vector3& position, unsigned int layer_id)
{
	// Create the unit
	PUnit* unit = new PUnit(position);
	// Assign it an ID
	unsigned int id;
	if (free_unit_ids.size() > 0) {
		id = free_unit_ids.back();
		free_unit_ids.pop_back();
	}
	else {
		id = next_unit_id++;
	}
	// Assign it to a layer
	layer_allocation[id] = layer_id;
	layers.at(layer_id)->AddUnit(unit, id);
	return id;
}

void PSystem::UpdateUnitTarget(unsigned int id, Vector3& target)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	layer->GetUnit(id)->UpdateTarget(target);
}

void PSystem::UpdateUnitPosition(unsigned int id, Vector3& position)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	layer->GetUnit(id)->UpdatePosition(position);
	layer->UpdateUnit(id);
}

void PSystem::DestroyUnit(unsigned int id)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(id));
	PUnit* unit = layer->GetUnit(id);
	layer->RemoveUnit(id);
	layer_allocation.erase(id);
	delete unit;
}

void PSystem::UpdateInteractions()
{
	std::vector<unsigned int> unit_ids;
	std::vector<glm::vec4> unit_buffer;
	std::vector<GLuint> count_buffer;
	std::vector<GLuint> index_buffer;
	std::vector<glm::vec4> neighbor_buffer;

	std::vector<unsigned int> leaders;

	for (auto unit_it = layer_allocation.begin(); unit_it != layer_allocation.end(); unit_it++) {
		unsigned int unit_id = (*unit_it).first;
		PUnitLayer* layer = layers.at(layer_allocation.at(unit_id));
		PUnit* current = layer->GetUnit(unit_id);
		std::unordered_set<unsigned int> &nearby = layer->Nearby(unit_id);

		unsigned int leader = 0;

		unit_ids.push_back(unit_id);
		unit_buffer.push_back(ToGLMVec4(current->GetPosition()));
		if (index_buffer.size() == 0) {
			index_buffer.push_back(0);
		}
		else {
			index_buffer.push_back(index_buffer.back() + count_buffer.back());
		}
		count_buffer.push_back(static_cast<unsigned int>(nearby.size()));

		if (nearby.size() > 0) {
			for (auto neighbor_it = nearby.begin(); neighbor_it != nearby.end(); neighbor_it++)
			{
				PUnit* neighbor = layer->GetUnit(*neighbor_it);

				float sqr_target_similarity = blaze::sqrLength(current->GetTarget() - neighbor->GetTarget());
				if (sqr_target_similarity < target_similarity_threshold) { // units have similar targets
					float sqr_current_target_distance = blaze::sqrLength(current->GetTarget() - current->GetPosition());
					float sqr_neighbor_target_distance = blaze::sqrLength(neighbor->GetPosition() - neighbor->GetTarget());
					if (sqr_neighbor_target_distance < sqr_current_target_distance) { // neighbor is closer than current
						leader = *neighbor_it;
					}
				}

				neighbor_buffer.push_back(ToGLMVec4(neighbor->GetPosition()));
			}
		}

		leaders.push_back(leader);
	}
	if (unit_buffer.size() == 0) {
		return;
	}

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, unit_ssbo_index, unit_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, unit_buffer.size() * 4 * sizeof(GLfloat), &unit_buffer[0], GL_STATIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, count_ssbo_index, count_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, count_buffer.size() * sizeof(GLuint), &count_buffer[0], GL_STATIC_DRAW);

	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index_ssbo_index, index_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, index_buffer.size() * sizeof(GLuint), &index_buffer[0], GL_STATIC_DRAW);

	if (neighbor_buffer.size() > 0) {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, neighbor_ssbo_index, neighbor_ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, neighbor_buffer.size() * 4 * sizeof(GLfloat), &neighbor_buffer[0], GL_STATIC_DRAW);
	}

	std::vector<glm::vec4> output;
	output.resize(unit_buffer.size());
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, output_ssbo_index, output_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, output.size() * 4 * sizeof(GLfloat), &output[0], GL_STATIC_DRAW);

	glDispatchCompute(static_cast<unsigned int>(unit_buffer.size()), 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, output.size() * 4 * sizeof(GLfloat), &output[0]);

	for (size_t i = 0; i < unit_ids.size(); i++)
	{
		glm::vec4& f = output[i];
		Vector3 computed_force{ f.x,f.y,f.z };
		Vector3 target_vector{ 0,0,0 };


		unsigned int id = unit_ids.at(i);

		PUnitLayer* layer = layers.at(layer_allocation.at(id));
		PUnit* current = layer->GetUnit(id);

		if (leaders.at(i) == 0) {
			// Calculate and save a path to the target
			std::vector<Vector3>* path = layer->GetPath(id);
			if (path == nullptr) {
				path = Pathfinding::a_star(layer->GetGraph(), current->GetPosition(), current->GetTarget());
				layer->SetPath(id, path);
			}
			Vector3 next = current->GetTarget();
			float sqr_next_distance = blaze::sqrLength(current->GetPosition() - next);
			if (!path->empty()) {
				next = path->back();
				sqr_next_distance = blaze::sqrLength(current->GetPosition() - next);
				while (sqr_next_distance < 49 && path->size() > 1) {
					path->pop_back();
					next = path->back();
					sqr_next_distance = blaze::sqrLength(current->GetPosition() - next);
				}
			}
			target_vector = next - current->GetPosition();
			target_vector = target_vector / std::sqrt(sqr_next_distance);
		}
		else {
			layer->ClearPath(id);
			PUnit* leader = GetUnit(leaders.at(i));
			target_vector = leader->GetPosition() - current->GetPosition();
			target_vector = target_vector / blaze::length(leader->GetPosition() - current->GetPosition());
		}

		target_vector = target_vector * target_factor;
		forces[id] = computed_force + target_vector;
	}
}

Vector3 PSystem::GetUnitForce(unsigned int id)
{
	if (forces.find(id) != forces.end()) {
		return forces.at(id);
	}
	else return Vector3{0, 0, 0};
}

void PSystem::CreateLayer(unsigned int layer_id)
{
	if (layers.find(layer_id) == layers.end()) {
		layers[layer_id] = new PUnitLayer();
	}
}

void ErrorCallback(int error, const char* description)
{
	printf("GLFW ERROR: %s\n", description);
}

PSystem::PSystem()
{
	// Init GLFW
	glfwSetErrorCallback(ErrorCallback);
	int glfw_err = glfwInit();
	if (glfw_err == GLFW_FALSE)
	{
		printf("ERROR INITIALIZING GLFW\n");
		glfwTerminate();
		return;
	}

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	GLFWwindow* offscreen_context = glfwCreateWindow(100, 100, "", NULL, NULL);
	glfwMakeContextCurrent(offscreen_context);

	// Init GLEW
	GLenum glew_err = glewInit();
	if (glew_err != GLEW_OK) {
		printf("ERROR INITIALIZING GLEW: %s\n", glewGetErrorString(glew_err));
		return;
	}

	// Load our compute shader
	std::string compute_shader_code;
	std::ifstream compute_shader_ifstream;

	try
	{
		compute_shader_ifstream.open("compute_shader.glsl");
		std::stringstream compute_shader_stream;
		compute_shader_stream << compute_shader_ifstream.rdbuf();
		compute_shader_ifstream.close();
		compute_shader_code = compute_shader_stream.str();
	}
	catch (std::ifstream::failure e) {
		printf("ERROR LOADING COMPUTE SHADER\n");
	}

	// Copy our shader code to OpenGL memory and compile
	const char* compute_shader_buffer = compute_shader_code.c_str();

	GLuint compute_shader_buffer_id;
	GLint success = 0;
	char log[512];

	compute_shader_buffer_id = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(compute_shader_buffer_id, 1, &compute_shader_buffer, NULL);
	glCompileShader(compute_shader_buffer_id);

	// Check if our shader successfully compiled
	glGetShaderiv(compute_shader_buffer_id, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(compute_shader_buffer_id, 512, NULL, log);
		printf(log);
	}

	// Link our shader program
	GLuint shader_program_id = glCreateProgram();
	glAttachShader(shader_program_id, compute_shader_buffer_id);
	glLinkProgram(shader_program_id);

	glGetProgramiv(shader_program_id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shader_program_id, 512, NULL, log);
		printf(log);
	}

	// Use our newly linked shader program
	glUseProgram(shader_program_id);

	unit_ssbo_index = glGetProgramResourceIndex(shader_program_id, GL_SHADER_STORAGE_BLOCK, "unit_buffer");
	count_ssbo_index = glGetProgramResourceIndex(shader_program_id, GL_SHADER_STORAGE_BLOCK, "count_buffer");
	index_ssbo_index = glGetProgramResourceIndex(shader_program_id, GL_SHADER_STORAGE_BLOCK, "index_buffer");
	neighbor_ssbo_index = glGetProgramResourceIndex(shader_program_id, GL_SHADER_STORAGE_BLOCK, "neighbor_buffer");
	output_ssbo_index = glGetProgramResourceIndex(shader_program_id, GL_SHADER_STORAGE_BLOCK, "output_buffer");

	glGenBuffers(1, &unit_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, unit_ssbo_index, unit_ssbo);
	glGenBuffers(1, &count_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, count_ssbo_index, count_ssbo);
	glGenBuffers(1, &index_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index_ssbo_index, index_ssbo);
	glGenBuffers(1, &neighbor_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, neighbor_ssbo_index, neighbor_ssbo);
	glGenBuffers(1, &output_ssbo);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, output_ssbo_index, output_ssbo);

	glShaderStorageBlockBinding(shader_program_id, unit_ssbo, 1);
	glShaderStorageBlockBinding(shader_program_id, count_ssbo, 2);
	glShaderStorageBlockBinding(shader_program_id, index_ssbo, 3);
	glShaderStorageBlockBinding(shader_program_id, neighbor_ssbo, 4);
	glShaderStorageBlockBinding(shader_program_id, output_ssbo, 5);
}

PUnit * PSystem::GetUnit(unsigned int unit_id)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(unit_id));
	PUnit* unit = layer->GetUnit(unit_id);
	return unit;
}
