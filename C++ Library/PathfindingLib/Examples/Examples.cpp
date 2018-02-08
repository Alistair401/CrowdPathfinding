#include "stdafx.h"
#include <PSystem.h>
#include <PGraph.h>
#include <random>
#include "blaze\Blaze.h"
#include "cairo\cairo.h"
#include "gtk\gtk.h"
#include "Unit.h"
#include "Target.h"
#include "DrawDebug.h"

int population = 20;
int target_count = 1;
int window_width = 800;
int window_height = 600;

int canvas_width = 0;
int canvas_height = 0;

std::vector<Unit*> units;
std::vector<Target*> targets;

std::vector<DrawDebug*> debug;

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
	for (int i = 0; i < debug.size(); i++)
	{
		debug.at(i)->Draw(cr);
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

void init_targets() {
	for (int i = 0; i < targets.size(); i++) {
		delete targets.at(i);
	}
	targets.clear();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);
	for (int i = 0; i < target_count; i++)
	{
		float x_pos = dis(gen) * canvas_width;
		float y_pos = dis(gen) * canvas_height;
		targets.push_back(new Target(x_pos, y_pos));
	}
}

void init_units() {
	for (int i = 0; i < units.size(); i++) {
		delete units.at(i);
	}
	units.clear();
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);
	for (int i = 0; i < population; i++)
	{
		float x_pos = dis(gen) * canvas_width;
		float y_pos = dis(gen) * canvas_height;
		Unit* u = new Unit(x_pos, y_pos);
		int random_target_index = std::rand() % target_count;
		Target* random_target = targets.at(random_target_index);
		u->SetTarget(random_target->x, random_target->y);
		units.push_back(u);
	}
}

void init_graph() {
	for (int i = 0; i < debug.size(); i++) {
		delete debug.at(i);
	}
	debug.clear();
	PSystem::GetInstance().CreateLayer(0);
	blaze::StaticVector<float, 3> origin{ 0.0, 0.0, 0.0 };
	blaze::StaticVector<float, 3> dimensions{ static_cast<float>(canvas_width), static_cast<float>(canvas_height), 0.0 };
	PSystem::GetInstance().InitGraph(0, origin, dimensions, 50);
	PGraph* graph = PSystem::GetInstance().GetGraph(0);
	//for (auto it = graph->graph->begin(); it != graph->graph->end(); it++) {
	//	blaze::StaticVector<float, 3> pos = (*it).second->position;
	//	debug.push_back(new DrawDebug(pos[0], pos[1]));
	//}
}

static void reset(GtkWidget* widget, gpointer data) {
	init_graph();
	init_targets();
	init_units();
}

int main(int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *main_container;
	GtkWidget *action_bar;
	GtkWidget *drawing_area;
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

	g_timeout_add(17, (GSourceFunc)tick, drawing_area);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), window_width, window_height);
	gtk_window_set_title(GTK_WINDOW(window), "Pathfinding Examples");

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}