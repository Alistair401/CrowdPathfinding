// Examples.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <cairo\cairo.h>
#include <gtkmm.h>

static void do_drawing(cairo_t *);

static gboolean on_draw_event(GtkWidget *widget, cairo_t *cr,
	gpointer user_data)
{
	do_drawing(cr);

	return FALSE;
}

static void do_drawing(cairo_t *cr)
{
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_set_line_width(cr, 0.5);
	cairo_translate(cr, 20, 20);
	cairo_arc(cr, 0, 0, 50, 0, 2 * 3.14);
	cairo_stroke_preserve(cr);
}


int main(int argc, char *argv[])
{
	GtkWidget *window;
	GtkWidget *darea;

	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	darea = gtk_drawing_area_new();
	gtk_container_add(GTK_CONTAINER(window), darea);

	g_signal_connect(G_OBJECT(darea), "draw",
		G_CALLBACK(on_draw_event), NULL);
	g_signal_connect(window, "destroy",
		G_CALLBACK(gtk_main_quit), NULL);

	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
	gtk_window_set_title(GTK_WINDOW(window), "GTK window");

	gtk_widget_show_all(window);

	gtk_main();

	return 0;
}

