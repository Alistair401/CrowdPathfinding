#include "stdafx.h"
#include "PSystem.h"
#include "PathSearch.h"
#include "GLFW\glfw3.h"
#include "GL\glew.h"
#include <fstream>
#include <sstream>

glm::vec4 ToGLMVec4(Vector3& vec3) {
	return glm::vec4(vec3.x, vec3.y, vec3.z, 0);
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
		std::vector<std::unordered_set<unsigned int>*> nearby = layer->Nearby(unit_id);

		unsigned int leader = 0;

		unit_ids.push_back(unit_id);
		unit_buffer.push_back(ToGLMVec4(current->GetPosition()));
		if (index_buffer.size() == 0) {
			index_buffer.push_back(0);
		}
		else {
			index_buffer.push_back(index_buffer.back() + count_buffer.back());
		}

		int neighbor_count = 0;

		if (nearby.size() > 0) {
			for (size_t i = 0; i < nearby.size(); i++)
			{
				for (auto neighbor_it = nearby.at(i)->begin(); neighbor_it != nearby.at(i)->end(); neighbor_it++) {
					if (*neighbor_it == unit_id) continue;
					PUnit* neighbor = layer->GetUnit(*neighbor_it);
					neighbor_count++;

					float sqr_target_similarity = SqrLength(current->GetTarget() - neighbor->GetTarget());
					if (sqr_target_similarity < target_similarity_threshold) { // units have similar targets
						float sqr_current_target_distance = SqrLength(current->GetTarget() - current->GetPosition());
						float sqr_neighbor_target_distance = SqrLength(neighbor->GetPosition() - neighbor->GetTarget());
						if (sqr_neighbor_target_distance < sqr_current_target_distance) { // neighbor is closer than current
							leader = *neighbor_it;
						}
					}
					neighbor_buffer.push_back(ToGLMVec4(neighbor->GetPosition()));
				}
			}
		}

		count_buffer.push_back(static_cast<GLuint>(neighbor_count));

		leaders.push_back(leader);
	}
	if (unit_buffer.size() == 0) {
		return;
	}

	unit_ssbo->Write(unit_buffer.size(), &unit_buffer[0]);
	count_ssbo->Write(count_buffer.size(), &count_buffer[0]);
	index_ssbo->Write(index_buffer.size(), &index_buffer[0]);

	if (neighbor_buffer.size() > 0) {
		neighbor_ssbo->Write(neighbor_buffer.size(), &neighbor_buffer[0]);
	}

	std::vector<glm::vec4> output;
	output.resize(unit_buffer.size());
	output_ssbo->Write(output.size(), &output[0]);

	glDispatchCompute(static_cast<unsigned int>(unit_buffer.size()), 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	output_ssbo->Read(output.size(), &output[0]);

	for (size_t i = 0; i < unit_ids.size(); i++)
	{
		glm::vec4& f = output[i];
		Vector3 flocking_vector{ f.x,f.y,f.z };

		unsigned int id = unit_ids.at(i);

		PUnitLayer* layer = layers.at(layer_allocation.at(id));
		PUnit* current = layer->GetUnit(id);

		// ===Leader following===
		Vector3 follow_vector{ 0,0,0 };

		if (leaders.at(i) == 0) {
			// Calculate and save a path to the target
			std::vector<Vector3>* path = layer->GetPath(id);
			if (path == nullptr) {
				path = PathSearch::AStar(layer->GetGraph(), current->GetPosition(), current->GetTarget());
				layer->SetPath(id, path);
			}
			Vector3 next = current->GetTarget();
			float sqr_next_distance = SqrLength(current->GetPosition() - next);
			if (!path->empty()) {
				next = path->back();
				sqr_next_distance = SqrLength(current->GetPosition() - next);
				while (sqr_next_distance < 80 && path->size() > 1) {
					path->pop_back();
					next = path->back();
					sqr_next_distance = SqrLength(current->GetPosition() - next);
				}
			}
			follow_vector = next - current->GetPosition();
		}
		else {
			layer->ClearPath(id);
			PUnit* leader = GetUnit(leaders.at(i));
			follow_vector = leader->GetPosition() - current->GetPosition();
		}
		follow_vector = follow_vector * follow_factor;

		// ===Obstacle avoidance===
		Vector3 avoidance_vector{ 0,0,0 };

		Vector3 estimated_position = current->GetPosition() + ((flocking_vector + follow_vector) * lookahead);

		PGraphNode* graph_node = layer->GetGraph()->NodeAt(estimated_position);
		if (graph_node->obstacle) {
			Vector3 separation = current->GetPosition() - graph_node->position;
			avoidance_vector = glm::normalize(separation) / glm::length(separation);
			avoidance_vector = avoidance_vector * avoidance_factor;
		}

		forces[id] = flocking_vector + avoidance_vector + follow_vector;
	}
}

Vector3 PSystem::GetUnitForce(unsigned int id)
{
	if (forces.find(id) != forces.end()) {
		return forces.at(id);
	}
	else return Vector3{ 0, 0, 0 };
}

void PSystem::CreateLayer(unsigned int layer_id)
{
	if (layers.find(layer_id) == layers.end()) {
		layers[layer_id] = new PUnitLayer();
	}
}

std::string PSystem::Stats()
{
	std::string path_count = "Path Calculations: " + std::to_string(PathSearch::CallCount());
	std::string node_count = "Nodes Evaluated: " + std::to_string(PathSearch::EvaluatedCount());
	return path_count + "\n" + node_count;
}

void PSystem::ResetStats()
{
	PathSearch::ResetStats();
}

void ErrorCallback(int error, const char* description)
{
	printf("GLFW ERROR: %s\n", description);
}

bool InitGLFW() {
	glfwSetErrorCallback(ErrorCallback);
	int glfw_err = glfwInit();
	if (glfw_err == GLFW_FALSE)
	{
		printf("ERROR INITIALIZING GLFW\n");
		glfwTerminate();
		return false;
	}

	glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
	GLFWwindow* offscreen_context = glfwCreateWindow(100, 100, "", NULL, NULL);
	glfwMakeContextCurrent(offscreen_context);

	return true;
}

bool InitGLEW() {
	GLenum glew_err = glewInit();
	if (glew_err != GLEW_OK) {
		printf("ERROR INITIALIZING GLEW: %s\n", glewGetErrorString(glew_err));
		return false;
	}
	printf("Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	return true;
}

GLuint LoadShader() {
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
		return false;
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

	return shader_program_id;
}

PSystem::PSystem()
{
	InitGLFW();
	InitGLEW();
	GLuint shader = LoadShader();

	glUseProgram(shader);

	unit_ssbo = new SSBO(shader, "unit_buffer", 4 * sizeof(GLfloat));
	count_ssbo = new SSBO(shader, "count_buffer", sizeof(unsigned int));
	index_ssbo = new SSBO(shader, "index_buffer", sizeof(unsigned int));
	neighbor_ssbo = new SSBO(shader, "neighbor_buffer", 4 * sizeof(GLfloat));
	output_ssbo = new SSBO(shader, "output_buffer", 4 * sizeof(GLfloat));
}

PUnit * PSystem::GetUnit(unsigned int unit_id)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(unit_id));
	PUnit* unit = layer->GetUnit(unit_id);
	return unit;
}
