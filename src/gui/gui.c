#include <stdio.h>
#include "../inc/const.h"
#include "gui.h"
#include "gui_gamemode.h"
#include "../game_logic/game_logic.h"
#include "../game.h"
#include <gtk/gtk.h>

GtkApplication *app;
GtkWidget *window;
GtkWidget *main_div;
GtkWidget *player_label;
GtkWidget *error_label;
int player = 1;

/**
 * @brief Function to destroy the window
 * @return void
 */
void destroy_window()
{
    if (window != NULL)
    {
        printf("Destroying window...\n");
        gtk_window_destroy(GTK_WINDOW(window));
    }
}

/**
 * @brief Function to quit the game
 * @return void
 */
void exit_game(GtkWidget *button, gpointer user_data)
{
    printf("Exiting game...\n");
    // We cast the data to a ServerResponse
    ServerResponse server_response = *(ServerResponse *)user_data;
    destroy_window();

    // Free the coordinates
    for (int i = 0; i < server_response.coordinates_size; i++)
    {
        free(server_response.coordinates[i]);
    }
}

/**
 * @brief Function to reset the game state
 * This function will reset all the global variables and the game logic state.
 * @return void
 */
void reset_game_state()
{
    printf("Resetting game state...\n");
    // Reset the board to its initial state
    init_board();

    // Reset player to 1
    player = 1;
}

/**
 * @brief The function to delete a button and the linked buttons
 * This function is called when a button is clicked
 * This function also calls the client to send the coordinates of the block to get the coordinates to delete
 * @param *button : The button clicked
 * @param user_data : The data passed to the function
 */
static void del_button(GtkWidget *button, gpointer user_data)
{
    printf("Deleting button...\n");

    // We get the coordinates of the button from the property
    const char *button_coordinate = gtk_widget_get_name(button);

    // validation of coordinates
    if (button_coordinate == NULL || strlen(button_coordinate) < 2 || 
        button_coordinate[0] < 'A' || button_coordinate[0] > 'I' || 
        button_coordinate[1] < '1' || button_coordinate[1] > '7') {
        printf("Invalid button coordinates: %s\n", button_coordinate);
        gtk_label_set_text(GTK_LABEL(error_label), "Invalid button coordinates");
        return;
    }

    // Call game logic with valid coordinates
    ServerResponse data = local_game_gui_logic(button_coordinate, player);

    player = data.player_num;

    // Updated the interface to show the current player
    gtk_label_set_text(GTK_LABEL(player_label), g_strdup_printf("Player %d", player));
    gtk_widget_add_css_class(player_label, "player_label");

    if (data.error == NULL)
    {
        // If no error, we reset the error label
        gtk_label_set_text(GTK_LABEL(error_label), "");

        // Delete buttons corresponding to destroyed coordinates
        for (int i = 0; i < data.coordinates_size; i++)
        {
            GtkWidget *table = gtk_widget_get_parent(button);
            GtkWidget *all_buttons[GAME_X * GAME_Y];

            for (int j = 0; j < GAME_X; j++)
            {
                for (int k = 0; k < GAME_Y; k++)
                {
                    all_buttons[j * GAME_Y + k] = gtk_grid_get_child_at(GTK_GRID(table), j + 1, k + 1);
                }
            }

            // Change the appearance of buttons corresponding to destroyed coordinates
            for (int j = 0; j < GAME_X * GAME_Y; j++)
            {
                if (strcmp(gtk_widget_get_name(all_buttons[j]), data.coordinates[i]) == 0)
                {
                    // Add a CSS class to change the color
                    gtk_widget_remove_css_class(all_buttons[j], "btn");  // Remove old class
                    gtk_widget_add_css_class(all_buttons[j], "btn_removed");  // Add new class
                }
            }
        }

        // Force a global redraw after updating buttons
        gtk_widget_queue_draw(window);

    }
    else
    {
        // If an error occurs, display the error message
        printf("Error occurred: %s\n", data.error);
        gtk_label_set_text(GTK_LABEL(error_label), data.error);

        if (data.is_game_over)
        {
            GtkWidget *exit_button = gtk_button_new_with_label("Exit");
            gtk_widget_add_css_class(exit_button, "btn_exit");
            g_signal_connect_data(exit_button, "clicked", G_CALLBACK(exit_game), &data, NULL, 0);
            gtk_widget_set_margin_top(exit_button, 10);
            gtk_grid_attach(GTK_GRID(main_div), exit_button, 1, 3, 1, 1);
        }
    }
}

/**
 * @brief The main window of the application
 * @param *app : The application
 * @param user_data : The data passed to the function
 * @return void
 */
static void active_gui(GtkApplication *app, gpointer user_data)
{
    printf("Activating GUI...\n");
    GtkWidget *button;
    GtkWidget *table;
    GtkWidget *center_box;
    char *css = ".btn{background: #ffc300; margin: 1px;} .btn:hover{border: 2px solid #D72638;} .btn_removed{opacity: 0; transition: opacity 0.2s;} .window {background: #001d3d;} .player_label {font-size: 20px; font-weight: bold;} .error_label {color: red; font-size: 18px; font-weight: bold;} .btn_exit {background: #00916E; color: black;} .player_label{color: white;} .coords{color: white;}";
    char *coordinate;

    // Creation of the window
    window = gtk_application_window_new(app);

    // Setting the parameters of the window (title and size)
    gtk_window_set_title(GTK_WINDOW(window), NAME);
    gtk_window_set_default_size(GTK_WINDOW(window), WINDOW_WIDTH, WINDOW_HEIGHT);

    // We add a css class to the window
    gtk_widget_add_css_class(window, "window");

    // We create a main_div
    main_div = gtk_grid_new();

    // Center the main_div in the window
    gtk_widget_set_halign(main_div, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_div, GTK_ALIGN_CENTER);

    // Set the number of columns in the grid
    gtk_grid_set_column_homogeneous(GTK_GRID(main_div), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(main_div), 10);

    // Add the CSS to the window
    GtkCssProvider *provider = gtk_css_provider_new();

    #if GTK_CHECK_VERSION(4, 12, 0)
        gtk_css_provider_load_from_string(provider, css);
    #else
        gtk_css_provider_load_from_data(provider, css, -1);
    #endif

    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // Add a widget to the main_div
    player_label = gtk_label_new(g_strdup_printf("Player %d", 1));
    gtk_grid_attach(GTK_GRID(main_div), player_label, 1, 0, 1, 1);
    gtk_widget_add_css_class(player_label, "player_label");

    // Add a margin under the label
    gtk_widget_set_margin_bottom(player_label, 10);

    // Add the error_label to the main_div
    error_label = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(main_div), error_label, 1, 2, 1, 1);
    gtk_widget_add_css_class(error_label, "error_label");

    // Add a margin over the label
    gtk_widget_set_margin_top(error_label, 10);

    // Center the error_label
    gtk_widget_set_halign(error_label, GTK_ALIGN_CENTER);

    // Create the game grid (7x9)
    table = gtk_grid_new();

    // We create yellow blocks to represent the game
    for (int i = 0; i < GAME_X; i++)
    {
        switch (i)
        {
        case 0:
            coordinate = "A";
            break;
        case 1:
            coordinate = "B";
            break;
        case 2:
            coordinate = "C";
            break;
        case 3:
            coordinate = "D";
            break;
        case 4:
            coordinate = "E";
            break;
        case 5:
            coordinate = "F";
            break;
        case 6:
            coordinate = "G";
            break;
        case 7:
            coordinate = "H";
            break;
        case 8:
            coordinate = "I";
            break;
        }

        GtkWidget *coord = gtk_label_new(coordinate);
        gtk_grid_attach(GTK_GRID(table), coord, i + 1, 0, 1, 1);
        gtk_widget_add_css_class(coord, "coords");

        for (int j = 0; j < GAME_Y; j++)
        {
            if (i == 0)
            {
                coord = gtk_label_new(g_strdup_printf("%d", j + 1));
                gtk_grid_attach(GTK_GRID(table), coord, 0, j + 1, 1, 1);
                gtk_widget_add_css_class(coord, "coords");
            }

            char *full_coordinate = g_strconcat(coordinate, g_strdup_printf("%d", j + 1), NULL);
            button = gtk_button_new_with_label(" ");
            gtk_widget_set_size_request(button, BUTTON_SIZE, BUTTON_SIZE);
            gtk_widget_add_css_class(button, "btn");

            gtk_widget_set_name(button, full_coordinate);

            printf("Button created with coordinate: %s\n", full_coordinate);

            g_signal_connect(button, "clicked", G_CALLBACK(del_button), NULL);
            gtk_grid_attach(GTK_GRID(table), button, i + 1, j + 1, 1, 1);
            g_free(full_coordinate);
        }
    }

    center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(center_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(center_box), table);

    gtk_grid_attach(GTK_GRID(main_div), center_box, 0, 1, 3, 1);
    gtk_window_set_child(GTK_WINDOW(window), main_div);

    gtk_window_present(GTK_WINDOW(window));
}

/**
 * @brief The main function of the application
 * @param argc : The number of arguments
 * @param argv : The arguments
 * @return int : The status of the application
 */
int launch_gui(int argc, char **argv)
{
    printf("Launching GUI...\n");
    int status;

    app = gtk_application_new("org.chompzilla.chomp", 0);
    g_signal_connect(app, "activate", G_CALLBACK(active_gui), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

/**
 * @brief Function to launch the game GUI when a client connects
 * 
 * @return void
 */
void launch_game_gui_on_client_connect() 
{
    printf("Launching game GUI on client connect...\n");
    destroy_window_gamemode();
    destroy_window_gamemode_wait();
    play_local_game_gui();
}

/**
 * @brief Function to update the GUI from the server
 * 
 * @param data The data to update the GUI with
 * @return gboolean TRUE if the function is successful, FALSE otherwise
 */
gboolean update_gui_from_server(gpointer data) 
{
    const char *coordinates = (const char *)data;
    printf("Updating GUI with move: %s\n", coordinates);

    if (coordinates == NULL || strlen(coordinates) < 2) {
        printf("Error: Invalid coordinates received from server\n");
        return FALSE;
    }
    printf("LES COORDONNES%c",coordinates[1]);
    char col = coordinates[0];
    int row = coordinates[1] - '0';

    if (row < 1 || row > 7) {
        printf("Error: Invalid row received from server: %d\n", row);
        return FALSE;
    }

    printf("Parsed move from server: %c%d\n", col, row);

    GtkWidget *table = gtk_widget_get_parent(gtk_widget_get_parent(window));
    GtkWidget *button_to_update = NULL;

    // Parse the grid to find the button
    for (int j = 0; j < GAME_X; j++) {
        for (int k = 0; k < GAME_Y; k++) {
            button_to_update = gtk_grid_get_child_at(GTK_GRID(table), j + 1, k + 1);
            if (button_to_update && strcmp(gtk_widget_get_name(button_to_update), coordinates) == 0) {
                // update button
                printf("Button found for %s, updating appearance.\n", coordinates);
                gtk_widget_add_css_class(button_to_update, "btn_del");
                gtk_widget_set_sensitive(button_to_update, FALSE);
                gtk_widget_queue_draw(button_to_update);
                break;
            }
        }
    }

    g_free((char *)data); // we empty the buffer
    return FALSE;
}
