#include "stdafx.h"
#include <stdlib.h>
#include "PathfindingLib.h"
#include "cairo\cairo.h"
#include "gtk\gtk.h"

static void do_drawing(cairo_t *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
	do_drawing(cr);

	return FALSE;
}

static void do_drawing(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
		CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 40.0);

	cairo_move_to(cr, 10.0, 50.0);
	cairo_show_text(cr, "Hello World");
}

static void draw_unit() {

}

static void draw_target() {

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

	g_signal_connect(G_OBJECT(drawing_area), "draw",
		G_CALLBACK(on_draw_event), NULL);
	g_signal_connect(window, "destroy",
		G_CALLBACK(gtk_main_quit), NULL);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
	gtk_window_set_title(GTK_WINDOW(window), "Pathfinding Examples");

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}