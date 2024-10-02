// Guards to prevent multiple inclusion
#ifndef GUI_H
#define GUI_H

#include <gtk/gtk.h>

// Define a struct to encapsulate the coordinates and error arrays
typedef struct
{
    int coordinates_size;
    char **coordinates;
    char *error;
    int player_num;
    bool is_game_over;
} ServerResponse;

// Function prototypes
void destroy_window();
void exit_game(GtkWidget *button, gpointer user_data);
void reset_game_state();
void restart_game(GtkWidget *button, gpointer user_data);
static void del_button(GtkWidget *button, gpointer user_data);
static void active_gui(GtkApplication *app, gpointer user_data);
int launch_gui(int argc, char **argv);
void launch_game_gui_on_client_connect();
gboolean update_gui_from_server(gpointer data);

#endif