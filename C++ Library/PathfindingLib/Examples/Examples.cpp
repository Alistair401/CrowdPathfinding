#include "stdafx.h"
#include <random>
#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <chrono>
#include <cairo\cairo.h>
#include <gtk\gtk.h>
#include <boost\tokenizer.hpp>
#include "Unit.h"
#include "Target.h"
#include "Obstacle.h"
#include "PSystem.h"

std::string environment_file = "environments/dense.csv";

int population_cap = 50;
float spawn_chance = 0.2f;
std::vector<Vector3> spawn_locations;

int target_count = 5;

int window_width = 800;
int window_height = 600;

int canvas_width = 0;
int canvas_height = 0;

float graph_scale = 25;

std::set<Unit*> units;
std::vector<Target*> targets;
std::vector<Obstacle*> obstacles;

GtkWidget *drawing_area;
guint update_timer;

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> spawn_chance_distribution(0, 1.0);
std::uniform_real_distribution<> spawn_location_distribution(-5.0, 5.0);

using TimePoint = std::chrono::time_point<std::chrono::steady_clock>;
using TimeDuration = std::chrono::duration<double>;
TimePoint start_time;

double benchmark_duration = 120.0;
bool benchmark_complete = true;

bool render = true;

unsigned int units_completed = 0;

static void do_drawing(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 0, 0, 0);

	for (auto unit: units)
	{
		unit->Draw(cr);
	}
	for (auto target : targets) {
		target->Draw(cr);
	}
	for (auto obstacle : obstacles) {
		obstacle->Draw(cr);
	}
}

static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
	if (render) do_drawing(cr);
	return FALSE;
}

void update_population() {
	std::vector<Unit*> to_remove;
	for (auto u : units)
	{
		if (u->IsComplete()) to_remove.push_back(u);
		u->Update();
	}
	PSystem::GetInstance().UpdateInteractions();
	for (auto u : to_remove)
	{
		units_completed++;
		units.erase(u);
	}

	if (spawn_locations.empty()) return;

	if (units.size() < population_cap && spawn_chance_distribution(gen) <= spawn_chance) {
		Vector3 random_position = spawn_locations.at(std::rand() % spawn_locations.size());
		Unit* u = new Unit(random_position[0] + spawn_location_distribution(gen), random_position[1] + spawn_location_distribution(gen));

		int random_target_index = std::rand() % target_count;
		Target* random_target = targets.at(random_target_index);
		u->SetTarget(random_target->x, random_target->y);
		units.emplace(u);
	}
}

void print_statistics() {
	std::cout << PSystem::GetInstance().Stats() << std::endl;
	std::cout << "Agent Tasks Completed: " << std::to_string(units_completed) << std::endl;
	PSystem::GetInstance().ResetStats();
}

static gboolean tick(GtkWidget* widget) {
	GtkAllocation alloc;
	gtk_widget_get_allocation(widget, &alloc);
	canvas_width = alloc.width;
	canvas_height = alloc.height;

	if (benchmark_complete) return TRUE;

	TimePoint current_time = std::chrono::high_resolution_clock::now();
	TimeDuration elapsed = current_time - start_time;
	if (elapsed.count() >= benchmark_duration) {
		print_statistics();
		benchmark_complete = true;
		return TRUE;
	}

	update_population();

	gtk_widget_queue_draw_area(widget, 0, 0, alloc.width, alloc.height);
	return TRUE;
}

void init_graph() {
	PSystem::GetInstance().CreateLayer(0U);
	Vector3 origin{ 0, 0, 0 };

	PGraph* graph = nullptr;
	float graph_width;
	float graph_height;
	std::vector<Vector3> obstacle_positions;
	spawn_locations.clear();

	if (environment_file == "") {
		graph_width = static_cast<float>(canvas_width);
		graph_height = static_cast<float>(canvas_height);
	}
	else {
		std::string line;
		std::ifstream file(environment_file);
		if (file.is_open()) {
			std::vector<std::vector<bool>> file_data;
			while (getline(file, line)) {
				std::vector<bool> line_data;
				boost::tokenizer<boost::escaped_list_separator <char>> tokenizer{ line };
				for (const auto &t : tokenizer) {
					line_data.push_back(t == "1");
				}
				file_data.push_back(line_data);
			}
			int rows = static_cast<int>(file_data.size());
			int columns = static_cast<int>(file_data.at(0).size());
			std::cout << "Loaded file - " << rows << "x" << columns << std::endl;
			graph_height = (rows - 1) * graph_scale;
			graph_width = (columns - 1) * graph_scale;
			for (int y = 0; y < rows; y++)
			{
				for (int x = 0; x < columns; x++)
				{
					if (file_data.at(y).at(x)) obstacle_positions.push_back(Vector3{ x * graph_scale,y * graph_scale,0 });
					else spawn_locations.push_back(Vector3{ x * graph_scale,y * graph_scale,0 });
				}
			}
		}
		else {
			std::cout << "Error loading .csv" << std::endl;
			graph_width = static_cast<float>(canvas_width);
			graph_height = static_cast<float>(canvas_height);
		}
	}

	Vector3 dimensions{ graph_width , graph_height , 0 };
	graph = PSystem::GetInstance().InitGraph(0U, origin, dimensions, graph_scale);

	for (size_t i = 0; i < obstacle_positions.size(); i++)
	{
		Vector3 pos = obstacle_positions.at(i);
		Obstacle* cairo_obstacle = new Obstacle(pos[0],pos[1],graph_scale,graph_scale);
		obstacles.push_back(cairo_obstacle);
		graph->NodeAt(pos)->obstacle = true;
	}
}

void init_targets() {
	for (int i = 0; i < targets.size(); i++) {
		delete targets.at(i);
	}
	targets.clear();

	for (int i = 0; i < target_count; i++)
	{
		Vector3 random_position = spawn_locations.at(std::rand() % spawn_locations.size());
		targets.push_back(new Target(random_position[0] + static_cast<float>(spawn_location_distribution(gen)), random_position[1] + static_cast<float>(spawn_location_distribution(gen))));
	}
}

void reset_units() {
	for (auto u : units)
	{
		delete u;
	}
	units.clear();
}

static void reset(GtkWidget* widget, gpointer data) {
	init_graph();
	init_targets();
	reset_units();
	units_completed = 0;
	benchmark_complete = false;
	start_time = std::chrono::high_resolution_clock::now();
}

int main(int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *main_container;
	GtkWidget *action_bar;
	GtkWidget *reset_button;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	main_container = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_container_add(GTK_CONTAINER(window), main_container);

	action_bar = gtk_action_bar_new();

	reset_button = gtk_button_new_with_label("Reset");

	gtk_action_bar_pack_start((GtkActionBar*)action_bar, reset_button);
	gtk_box_pack_start((GtkBox*)main_container, action_bar, FALSE, FALSE, 0);

	drawing_area = gtk_drawing_area_new();
	gtk_box_pack_start((GtkBox*)main_container, drawing_area, TRUE, TRUE, 0);

	g_signal_connect(G_OBJECT(drawing_area), "draw", G_CALLBACK(draw_callback), NULL);
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(reset_button, "clicked", G_CALLBACK(reset), NULL);

	update_timer = g_timeout_add(17, (GSourceFunc)tick, drawing_area);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), window_width, window_height);
	gtk_window_set_title(GTK_WINDOW(window), "Pathfinding Examples");

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}