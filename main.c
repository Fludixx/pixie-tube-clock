#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <pthread.h>
#include <time.h>
#include "X11/Xlib.h"
#define image_width 130
#define clock_precision 8

static GtkWidget* window;
static GtkWidget* fixed;
static GtkWidget* display[clock_precision+1];

void update_clock(char clock_buffer[]) {
	for(int i = 0; i != clock_precision; i++) {
		char path_buffer[12];
		sprintf(path_buffer, "assets/%c.png", clock_buffer[i]);
		GtkWidget* display_to_destroy = display[i];
		display[i] = gtk_image_new_from_file(path_buffer);
		gtk_fixed_put(GTK_FIXED(fixed), display[i], image_width*i, 0);
		gtk_widget_show_all(display[i]);
		if(GTK_IS_WIDGET(display_to_destroy)) {
			gtk_widget_destroy(display_to_destroy);
		}
	}
}

void update_clock_thread_safe(char clock_buffer[]) {
	g_idle_add(update_clock, clock_buffer);
}

void* clock_updater(void *vargp) { 
	while TRUE {
		time_t rawtime;
		struct tm *ptm;
		time(&rawtime);
		ptm = localtime(&rawtime);
		char clock_buffer[9];
		sprintf(clock_buffer, "%02d:%02d:%02d", ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
		update_clock_thread_safe(clock_buffer);
		sleep(1);
	}
}

int main(int argc, char* argv[]) {
	XInitThreads();
	gtk_init(&argc, &argv);

	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	fixed = gtk_fixed_new();

	gtk_container_add(GTK_CONTAINER(window), fixed);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_widget_show_all(window);

	pthread_t updater;
	pthread_create(&updater, NULL, clock_updater, NULL); 

	gtk_main();
} 
