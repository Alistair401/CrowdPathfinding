#include "stdafx.h"
#include <PSystem.h>
#include <PGraph.h>
#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <blaze\Blaze.h>
#include <cairo\cairo.h>
#include <gtk\gtk.h>
#include <boost\tokenizer.hpp>
#include "Unit.h"
#include "Target.h"
#include "DrawDebug.h"

std::string environment_file = "sparse.csv";

int population = 40;
int target_count = 5;

int window_width = 800;
int window_height = 600;

int canvas_width = 0;
int canvas_height = 0;

float graph_scale = 16;

std::vector<Unit*> units;
std::vector<Target*> targets;

GtkWidget *drawing_area;
guint update_timer;

static void do_drawing(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 0, 0, 0);
	for (int i = 0; i < units.size(); i++)
	{
		units.at(i)->Draw(cr);
	}
	for (int i = 0; i < targets.size(); i++)
	{
		targets.at(i)->Draw(cr);
	}
}

static gboolean draw_callback(GtkWidget *widget, cairo_t *cr, gpointer data) {
	do_drawing(cr);
	return FALSE;
}

void update_units() {
	for (int i = 0; i < units.size(); i++)
	{
		units.at(i)->Update();
	}
}

static gboolean tick(GtkWidget* widget) {
	GtkAllocation alloc;
	gtk_widget_get_allocation(widget, &alloc);
	canvas_width = alloc.width;
	canvas_height = alloc.height;

	update_units();

	gtk_widget_queue_draw_area(widget, 0, 0, alloc.width, alloc.height);
	return TRUE;
}

std::vector<blaze::StaticVector<float, 3>> init_graph() {
	PSystem::GetInstance().CreateLayer(0U);
	blaze::StaticVector<float, 3> origin{ 0, 0, 0 };

	PGraph* graph = nullptr;
	float graph_width;
	float graph_height;
	std::vector<blaze::StaticVector<float, 3>> obstacles;

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
				for (int x = 0; x  < columns; x++)
				{
					if (file_data.at(y).at(x)) obstacles.push_back(blaze::StaticVector<float, 3>{x * graph_scale,y * graph_scale,0});
				}
			}
		}
		else {
			std::cout << "Error loading .csv" << std::endl;
			graph_width = static_cast<float>(canvas_width);
			graph_height = static_cast<float>(canvas_height);
		}
	}

	
	blaze::StaticVector<float, 3> dimensions{ graph_width , graph_height , 0 };
	graph = PSystem::GetInstance().InitGraph(0U, origin, dimensions, graph_scale);

	for (size_t i = 0; i < obstacles.size(); i++)
	{
		graph->NodeAt(obstacles.at(i))->obstacle = true;
	}

	std::vector<blaze::StaticVector<float, 3>> open_positions;
	for (auto it = graph->graph->begin(); it != graph->graph->end(); it++) {
		if (!(*it).second->obstacle) open_positions.push_back((*it).second->position);
	}
	return open_positions;
}

void init_targets(std::vector<blaze::StaticVector<float, 3>>& open_positions) {
	for (int i = 0; i < targets.size(); i++) {
		delete targets.at(i);
	}
	targets.clear();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-1.0, 1.0);
	for (int i = 0; i < target_count; i++)
	{
		blaze::StaticVector<float, 3> random_position = open_positions.at(std::rand() % open_positions.size());
		targets.push_back(new Target(random_position[0] + static_cast<float>(dis(gen)), random_position[1] + static_cast<float>(dis(gen))));
	}
}

void init_units(std::vector<blaze::StaticVector<float, 3>>& open_positions) {
	for (int i = 0; i < units.size(); i++) {
		delete units.at(i);
	}
	units.clear();

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-1.0, 1.0);
	for (int i = 0; i < population; i++)
	{
		blaze::StaticVector<float, 3> random_position = open_positions.at(std::rand() % open_positions.size());
		Unit* u = new Unit(random_position[0] + dis(gen), random_position[1] + dis(gen));

		int random_target_index = std::rand() % target_count;
		Target* random_target = targets.at(random_target_index);
		u->SetTarget(random_target->x, random_target->y);
		units.push_back(u);
	}
}

static void reset(GtkWidget* widget, gpointer data) {
	std::vector<blaze::StaticVector<float, 3>> open = init_graph();
	init_targets(open);
	init_units(open);
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