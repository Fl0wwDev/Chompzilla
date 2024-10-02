#ifndef SERVER_GUI_H
#define SERVER_GUI_H

#include <gtk/gtk.h>

void destroy_window_server_gui();
void exit_game_server_gui(GtkWidget *button, gpointer user_data);
void reset_game_state_server_gui();
void *start_server_server_gui(void *arg);
int launch_gui_server_gui();
void restart_game_state_server_gui(GtkWidget *button, gpointer user_data);
void send_coordinates_to_client(const char *coordinates);
void set_buttons_state_server_gui(gboolean state);
static void del_button_server_gui(GtkWidget *button, gpointer user_data);
static void active_gui_server_gui(GtkApplication *app_server_gui, gpointer user_data, gboolean client_starts);
static void active_gui_server_gui(GtkApplication *app_server_gui, gpointer user_data, gboolean client_starts);
void *handle_client_server_gui(void *arg);
int network_server_gui(unsigned short port);

#endif