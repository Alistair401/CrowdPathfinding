#include "stdafx.h"
#include "PSystem.h"
#include "PathSearch.h"
#include "GLFW\glfw3.h"
#include "GL\glew.h"
#include <fstream>
#include <sstream>

// Define structs to match those used in the SSBO
struct ssbo_unit_t {
	glm::vec3 position;
	GLfloat padding0;
	glm::vec3 target;
	GLuint neighbor_count;
	GLuint neighbor_start_index;
	GLuint padding1;
	GLuint padding2;
	GLuint padding3;
};
static const size_t ssbo_unit_size = (7 * sizeof(GLfloat)) + sizeof(GLuint) + (4 * sizeof(GLuint));
static_assert(sizeof(ssbo_unit_t) == ssbo_unit_size, "Padding Required");

struct ssbo_neighbor_t {
	glm::vec3 position;
	GLfloat padding0;
	glm::vec3 target;
	GLfloat padding1;
};
static const size_t ssbo_neighbor_size = 8 * sizeof(GLfloat);
static_assert(sizeof(ssbo_neighbor_t) == ssbo_neighbor_size, "Padding Required");

struct ssbo_result_t {
	glm::vec3 force;
	GLuint request_path;
};
static const size_t ssbo_result_size = (3 * sizeof(GLfloat)) + sizeof(GLuint);
static_assert(sizeof(ssbo_result_t) == ssbo_result_size, "Padding Required");

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
	std::vector<ssbo_unit_t> unit_buffer;
	std::vector<ssbo_neighbor_t> neighbor_buffer;

	GLuint current_neighbor_index = 0;

	for (auto unit_it = layer_allocation.begin(); unit_it != layer_allocation.end(); unit_it++) {
		unsigned int unit_id = (*unit_it).first;
		PUnitLayer* layer = layers.at(layer_allocation.at(unit_id));
		PUnit* current = layer->GetUnit(unit_id);
		std::vector<std::unordered_set<unsigned int>*> nearby = layer->Nearby(unit_id);

		GLuint neighbor_count = 0;

		if (nearby.size() > 0) {
			for (size_t i = 0; i < nearby.size(); i++)
			{
				for (auto neighbor_it = nearby.at(i)->begin(); neighbor_it != nearby.at(i)->end(); neighbor_it++) {
					if (*neighbor_it == unit_id) continue;

					PUnit* neighbor = layer->GetUnit(*neighbor_it);
					neighbor_count++;
					neighbor_buffer.push_back(ssbo_neighbor_t{ neighbor->GetPosition(),0, neighbor->GetTarget() });
				}
			}
		}

		unit_ids.push_back(unit_id);
		unit_buffer.push_back(ssbo_unit_t{ current->GetPosition(),0,current->GetTarget(),neighbor_count,current_neighbor_index });
		current_neighbor_index += neighbor_count;
	}

	if (unit_buffer.size() == 0) {
		return;
	}

	unit_ssbo->Write(unit_buffer.size(), &unit_buffer[0]);

	if (neighbor_buffer.size() > 0) {
		neighbor_ssbo->Write(neighbor_buffer.size(), &neighbor_buffer[0]);
	}

	std::vector<ssbo_result_t> output;
	output.resize(unit_buffer.size());
	output_ssbo->Write(output.size(), &output[0]);

	glDispatchCompute(static_cast<unsigned int>(unit_buffer.size()), 1, 1);

	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	output_ssbo->Read(output.size(), &output[0]);

	for (size_t i = 0; i < unit_ids.size(); i++)
	{
		ssbo_result_t& result = output[i];

		unsigned int id = unit_ids.at(i);

		PUnitLayer* layer = layers.at(layer_allocation.at(id));
		PUnit* current = layer->GetUnit(id);

		// ===Path Request Fulfillment===
		Vector3 path_following_vector{ 0,0,0 };

		if (result.request_path == 1) {
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
			path_following_vector = next - current->GetPosition();
			path_following_vector = path_following_vector * path_following_factor;
		}

		// ===Obstacle avoidance===
		Vector3 avoidance_vector{ 0,0,0 };

		Vector3 estimated_position = current->GetPosition() + ((result.force + path_following_vector) * lookahead);

		PGraphNode* graph_node = layer->GetGraph()->NodeAt(estimated_position);
		if (graph_node->obstacle) {
			Vector3 separation = current->GetPosition() - graph_node->position;
			avoidance_vector = glm::normalize(separation) / glm::length(separation);
			avoidance_vector = avoidance_vector * avoidance_factor;
		}

		forces[id] = result.force + path_following_vector + avoidance_vector;
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

	unit_ssbo = new SSBO(shader, "unit_buffer", ssbo_unit_size);
	neighbor_ssbo = new SSBO(shader, "neighbor_buffer", ssbo_neighbor_size);
	output_ssbo = new SSBO(shader, "output_buffer", ssbo_result_size);
}

PUnit * PSystem::GetUnit(unsigned int unit_id)
{
	PUnitLayer* layer = layers.at(layer_allocation.at(unit_id));
	PUnit* unit = layer->GetUnit(unit_id);
	return unit;
}
