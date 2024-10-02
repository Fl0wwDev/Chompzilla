#ifndef CLIENT_GUI_H
#define CLIENT_GUI_H

#include <gtk/gtk.h>

void destroy_window_client_gui();
void exit_game_client_gui();
void reset_game_state_client_gui();
void *start_client_send_client_gui(void *arg);
static void del_button_client_gui(GtkWidget *button, gpointer user_data);
gboolean update_label_text_client_gui(gpointer data);
gboolean update_error_label_text_client_gui(gpointer data);
static void active_gui_client_gui(GtkApplication *app_client_gui, gpointer user_data);
void *start_client_receive_client_gui(void *arg);
int launch_gui_client_gui();
int network_client_gui(const char *server_ip, unsigned short port);

#endif