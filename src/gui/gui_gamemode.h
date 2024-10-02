// Guards to prevent multiple inclusion
#ifndef GUI_GAMEMODE_H
#define GUI_GAMEMODE_H

#include <gtk/gtk.h>

typedef struct
{
    GtkWidget* entry_ip;
    GtkWidget* entry_port;
} IpPortGtk;

void destroy_window_gamemode_wait();
void start_button_server_gui(GtkWidget* button, gpointer user_data);
void destroy_window_gamemode();
void play_local_game_gui_button(GtkWidget *button, gpointer user_data);
void play_network_game_gui_button(GtkWidget* button, gpointer user_data);
static void play_vs_ia_gui_button(GtkWidget *button, gpointer user_data);
static void active_gui_gamemode(GtkApplication *app, gpointer user_data);
int launch_gui_gamemode(int argc, char **argv);
#endif
