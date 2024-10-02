#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include "server_gui.h"
#include "../inc/const.h"

GtkApplication *app_server_gui;
GtkWidget *window_server_gui;
GtkWidget *main_div_server_gui;
GtkWidget *player_label_server_gui;
GtkWidget *error_label_server_gui;
GtkWidget *end_game_server_gui;
GtkWidget *table_server_gui;
int player_server_gui = 1;
int client_socket_server_gui = -1;
int buttons_deleted_server_gui = 0;
int is_local_player_turn_server_gui = 1;
// Mutex to protect the turn
pthread_mutex_t turn_mutex_server_gui = PTHREAD_MUTEX_INITIALIZER;
unsigned short port_server_gui;

/**
 * @brief Function to destroy the window
 * @return void
 */
void destroy_window_server_gui()
{
    if (window_server_gui != NULL)
    {
        printf("Destroying window...\n");
        gtk_window_destroy(GTK_WINDOW(window_server_gui));
    }
}

/**
 * @brief Function to quit the game
 * @return void
 */
void exit_game_server_gui(GtkWidget *button, gpointer user_data)
{
    printf("Exiting game...\n");
    destroy_window_server_gui();
}

/**
 * @brief Function to reset the game state
 * This function will reset all the global variables and the game logic state.
 * @return void
 */
void reset_game_state_server_gui()
{
    printf("Resetting game state...\n");
    player_server_gui = 1;
    // Reset the counter
    buttons_deleted_server_gui = 0;
    // Reset the turn
    is_local_player_turn_server_gui = 0;
}

/**
 * @brief Function to start the server GUI
 *
 * @param arg : The arguments
 * @return void*
 */
void *start_server_server_gui(void *arg)
{
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created successfully.\n");

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    printf("Socket options set successfully.\n");

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_server_gui);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket bound to port %d.\n", port_server_gui);

    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Server is now listening for connections...\n");

    while (1)
    {
        if ((client_socket_server_gui = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
        {
            perror("accept");
            continue;
        }
        printf("Connection established with client.\n");
        printf("Client connected from IP: %s, Port: %d\n", inet_ntoa(address.sin_addr), ntohs(address.sin_port));

        pthread_t client_thread;
        int *pclient = malloc(sizeof(int));
        *pclient = client_socket_server_gui;
        pthread_create(&client_thread, NULL, handle_client_server_gui, pclient);
        pthread_detach(client_thread);
    }
}

/**
 * @brief The main function of the application
 *
 * @return int : The status of the application
 */
int launch_gui_server_gui()
{
    printf("Launching GUI...\n");
    int status;

    pthread_t server_thread;
    pthread_create(&server_thread, NULL, start_server_server_gui, NULL);

    app_server_gui = gtk_application_new("org.chompzilla.chompserver", 0);
    g_signal_connect(app_server_gui, "activate", G_CALLBACK(active_gui_server_gui), NULL);
    status = g_application_run(G_APPLICATION(app_server_gui), 0, NULL);
    g_object_unref(app_server_gui);

    return status;
}

/**
 * @brief Function to restart the game
 *
 * @param button : The button clicked
 * @param user_data : The data passed to the function
 * @return void
 */
void restart_game_state_server_gui(GtkWidget *button, gpointer user_data)
{
    printf("Restarting game...\n");
    reset_game_state_server_gui();
    destroy_window_server_gui();
    launch_gui_server_gui(0, NULL);
}

void send_coordinates_to_client(const char *coordinates)
{
    if (client_socket_server_gui < 0)
    {
        printf("No client connected.\n");
        return;
    }

    // Send the coordinates to the client
    ssize_t bytes_sent = send(client_socket_server_gui, coordinates, strlen(coordinates), 0);
    if (bytes_sent < 0)
    {
        printf("Failed to send coordinates to client.\n");
    }
    else
    {
        printf("Successfully sent %ld bytes to client.\n", bytes_sent);
    }
}

/**
 * @brief Function to set the state of the buttons
 *
 * @param state : The state of the buttons
 * @return void
 */
void set_buttons_state_server_gui(gboolean state)
{
    for (int i = 0; i < GAME_X; i++)
    {
        for (int j = 0; j < GAME_Y; j++)
        {
            GtkWidget *btn = gtk_grid_get_child_at(GTK_GRID(table_server_gui), i + 1, j + 1);
            if (btn && !gtk_widget_has_css_class(btn, "btn_del"))
            {
                // Enable or disable the button
                gtk_widget_set_sensitive(btn, state);
            }
        }
    }
}

/**
 * @brief The function to delete a button and the linked buttons
 * This function is called when a button is clicked
 * This function also calls the client to send the coordinates of the block to get the coordinates to delete
 *
 * @param *button : The button clicked
 * @param user_data : The data passed to the function
 * @return void
 */
static void del_button_server_gui(GtkWidget *button, gpointer user_data)
{
    printf("Deleting button...\n");
    const char *button_coordinate = gtk_widget_get_name(button);
    printf("Button clicked: %s\n", button_coordinate);

    int x = button_coordinate[0] - 'A';
    int y = button_coordinate[1] - '1';
    printf("Parsed coordinates: x = %d, y = %d\n", x, y);

    GtkWidget *table_server_gui = gtk_widget_get_parent(button);
    if (!GTK_IS_GRID(table_server_gui))
    {
        printf("Error: The parent widget is not a GtkGrid.\n");
        return;
    }

    int new_buttons_to_delete = 0;

    printf("Total buttons deleted so far: %d\n", buttons_deleted_server_gui);

    for (int i = x; i < GAME_X; i++)
    {
        for (int j = y; j < GAME_Y; j++)
        {
            GtkWidget *btn = gtk_grid_get_child_at(GTK_GRID(table_server_gui), i + 1, j + 1);

            if (btn && !gtk_widget_has_css_class(btn, "btn_del"))
            {
                new_buttons_to_delete++;
                printf("Button at [%d, %d] can be deleted (total new: %d)\n", i, j, new_buttons_to_delete);

                if (new_buttons_to_delete > 5)
                {
                    printf("new_buttons_to_delete: %d\n", new_buttons_to_delete);
                    printf("Cannot delete more than 5 buttons.\n");
                    // Update the error label
                    gtk_label_set_text(GTK_LABEL(error_label_server_gui), "Cannot delete more than 5 buttons.");
                    gtk_widget_add_css_class(error_label_server_gui, "error_label");
                    return;
                }
                else
                {
                    // Reset the error label
                    gtk_label_set_text(GTK_LABEL(error_label_server_gui), "");
                }
            }
            else if (btn && gtk_widget_has_css_class(btn, "btn_del"))
            {
                printf("Button at [%d, %d] already deleted or not found. Ignoring it.\n", i, j);
            }
        }
    }

    printf("Potential new buttons to delete in this move: %d\n", new_buttons_to_delete);

    // If the number of buttons is in the limit
    for (int i = x; i < GAME_X; i++)
    {
        for (int j = y; j < GAME_Y; j++)
        {
            GtkWidget *btn = gtk_grid_get_child_at(GTK_GRID(table_server_gui), i + 1, j + 1);
            if (btn && !gtk_widget_has_css_class(btn, "btn_del"))
            {
                gtk_widget_add_css_class(btn, "btn_del");
                g_signal_handlers_disconnect_by_func(btn, G_CALLBACK(del_button_server_gui), NULL);
                buttons_deleted_server_gui++;
                printf("Deleted button at [%d, %d]. Total deleted now: %d\n", i, j, buttons_deleted_server_gui);
                if (buttons_deleted_server_gui == ROWS * COLS)
                {
                    char *end_game_message = "";
                    if (player_server_gui == 2)
                    {
                        end_game_message = "You have lost :(";
                    }
                    else
                    {
                        end_game_message = "You have won :)";
                    }

                    gtk_label_set_text(GTK_LABEL(end_game_server_gui), g_strdup_printf("Player %d has lost.\n%s", player_server_gui, end_game_message));
                    // Add an exit button
                    GtkWidget *exit_button = gtk_button_new_with_label("Exit");
                    gtk_widget_add_css_class(exit_button, "btn_exit");
                    g_signal_connect(exit_button, "clicked", G_CALLBACK(exit_game_server_gui), NULL);
                    gtk_grid_attach(GTK_GRID(main_div_server_gui), exit_button, 1, 4, 1, 1);
                }
            }
        }
    }

    // Update of the player label
    player_server_gui = (player_server_gui % 2) + 1;
    gtk_label_set_text(GTK_LABEL(player_label_server_gui), g_strdup_printf("Player %d", player_server_gui));
    gtk_widget_add_css_class(player_label_server_gui, "player_label");

    // Disable the buttons after the local player's turn
    set_buttons_state_server_gui(FALSE);

    // Send the coordinates to the client
    send_coordinates_to_client(button_coordinate);
    printf("Coordinates sent to client: %s\n", button_coordinate);
}

/**
 * @brief The main window of the application
 *
 * @param *app_server_gui : The application
 * @param user_data : The data passed to the function
 * @param client_starts : The client starts
 * @return void
 */
static void active_gui_server_gui(GtkApplication *app_server_gui, gpointer user_data, gboolean client_starts)
{
    printf("Activating GUI...\n");
    GtkWidget *button;
    GtkWidget *center_box;
    char *css = ".btn {background: #ffc300; margin: 1px;} .btn:hover{border: 2px solid #D72638;} .window {background: #001d3d;} .player_label {font-size: 20px; font-weight: bold;} .error_label {color: red; font-size: 18px; font-weight: bold;} .btn_exit {background: #00916E; color: black;} .player_label{color: white;} .coords{color: white;} .btn_del{opacity: 0; transition: opacity 0.2s;}";

    window_server_gui = gtk_application_window_new(app_server_gui);
    gtk_window_set_title(GTK_WINDOW(window_server_gui), NAME);
    gtk_window_set_default_size(GTK_WINDOW(window_server_gui), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_widget_add_css_class(window_server_gui, "window");

    main_div_server_gui = gtk_grid_new();
    gtk_widget_set_halign(main_div_server_gui, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_div_server_gui, GTK_ALIGN_CENTER);
    gtk_grid_set_column_homogeneous(GTK_GRID(main_div_server_gui), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(main_div_server_gui), 10);

    GtkCssProvider *provider = gtk_css_provider_new();
#if GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(provider, css);
#else
    gtk_css_provider_load_from_data(provider, css, -1);
#endif
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    player_label_server_gui = gtk_label_new(g_strdup_printf("Player %d", 1));
    gtk_grid_attach(GTK_GRID(main_div_server_gui), player_label_server_gui, 1, 0, 1, 1);
    gtk_widget_add_css_class(player_label_server_gui, "player_label");
    gtk_widget_set_margin_bottom(player_label_server_gui, 10);

    table_server_gui = gtk_grid_new();
    for (int i = 0; i < GAME_X; i++)
    {
        char *coordinate;
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
        gtk_grid_attach(GTK_GRID(table_server_gui), coord, i + 1, 0, 1, 1);
        gtk_widget_add_css_class(coord, "coords");
        gtk_widget_set_margin_bottom(coord, 10);

        for (int j = 0; j < GAME_Y; j++)
        {
            if (i == 0)
            {
                coord = gtk_label_new(g_strdup_printf("%d", j + 1));
                gtk_grid_attach(GTK_GRID(table_server_gui), coord, 0, j + 1, 1, 1);
                gtk_widget_add_css_class(coord, "coords");
                gtk_widget_set_margin_end(coord, 10);
            }
            char *full_coordinate = g_strconcat(coordinate, g_strdup_printf("%d", j + 1), NULL);
            button = gtk_button_new_with_label(" ");
            gtk_widget_set_size_request(button, BUTTON_SIZE, BUTTON_SIZE);
            gtk_widget_add_css_class(button, "btn");
            gtk_widget_set_name(button, full_coordinate);
            g_signal_connect(button, "clicked", G_CALLBACK(del_button_server_gui), NULL);
            printf("Button created with coordinate: %s\n", full_coordinate);
            gtk_grid_attach(GTK_GRID(table_server_gui), button, i + 1, j + 1, 1, 1);
            g_free(full_coordinate);
        }
    }

    center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(center_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(center_box), table_server_gui);

    // Add the error label
    error_label_server_gui = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(main_div_server_gui), error_label_server_gui, 1, 2, 1, 1);
    gtk_widget_add_css_class(error_label_server_gui, "error_label");
    gtk_widget_set_margin_top(error_label_server_gui, 10);

    // Add the end game label
    end_game_server_gui = gtk_label_new("");
    gtk_grid_attach(GTK_GRID(main_div_server_gui), end_game_server_gui, 1, 3, 1, 1);
    gtk_widget_add_css_class(end_game_server_gui, "player_label");
    gtk_widget_set_margin_top(end_game_server_gui, 10);
    gtk_widget_set_margin_bottom(end_game_server_gui, 10);

    gtk_grid_attach(GTK_GRID(main_div_server_gui), center_box, 0, 1, 3, 1);
    gtk_window_set_child(GTK_WINDOW(window_server_gui), main_div_server_gui);
    gtk_window_present(GTK_WINDOW(window_server_gui));

    // If the client starts, disable the buttons
    if (client_starts)
    {
        printf("Client starts, disabling buttons.\n");
        set_buttons_state_server_gui(FALSE);
        is_local_player_turn_server_gui = 0;
    }
    else
    {
        printf("Server starts, enabling buttons.\n");
        set_buttons_state_server_gui(TRUE);
        is_local_player_turn_server_gui = 1;
    }
}

/**
 * @brief The main window of the application
 *
 * @param arg The arguments
 * @return void*
 */
void *handle_client_server_gui(void *arg)
{
    int client_socket_server_gui = *((int *)arg);
    free(arg);
    char buffer[BUFFER_SIZE] = {0};

    // Infinite loop to handle the client
    while (1)
    {
        int total_bytes_read = 0;
        int bytes_read;

        // Reading of the client's data
        while ((bytes_read = read(client_socket_server_gui, buffer + total_bytes_read, BUFFER_SIZE - total_bytes_read - 1)) > 0)
        {
            total_bytes_read += bytes_read;
            // We verify that the string is complete
            buffer[total_bytes_read] = '\0';
            printf("Received %d bytes from client: %s\n", total_bytes_read, buffer);
            break;
        }

        if (bytes_read == 0)
        {
            printf("Client disconnected.\n");
            close(client_socket_server_gui);
            return NULL;
        }

        if (bytes_read < 0)
        {
            perror("Error reading from client");
            close(client_socket_server_gui);
            return NULL;
        }

        // Processing of the client's data
        int x = buffer[0] - 'A';
        int y = buffer[1] - '1';
        printf("Parsed coordinates: x = %d, y = %d\n", x, y);

        if (!GTK_IS_GRID(table_server_gui))
        {
            printf("Error: The table widget is not a GtkGrid.\n");
            continue;
        }

        int new_buttons_to_delete = 0;

        // Delete the buttons
        for (int i = x; i < GAME_X; i++)
        {
            for (int j = y; j < GAME_Y; j++)
            {
                GtkWidget *btn = gtk_grid_get_child_at(GTK_GRID(table_server_gui), i + 1, j + 1);
                if (btn && !gtk_widget_has_css_class(btn, "btn_del"))
                {
                    gtk_widget_add_css_class(btn, "btn_del");
                    g_signal_handlers_disconnect_by_func(btn, G_CALLBACK(del_button_server_gui), NULL);
                    buttons_deleted_server_gui++;
                    new_buttons_to_delete++;
                    printf("Deleted button at [%d, %d]. Total deleted now: %d\n", i, j, buttons_deleted_server_gui);
                }
            }
        }
        printf("Total buttons deleted in this move: %d\n", new_buttons_to_delete);

        // We verify that we have not reached the end of the game
        if (buttons_deleted_server_gui == ROWS * COLS)
        {
            // We check if the current player has win or lose
            char *end_game_message = "";
            if (player_server_gui == 2)
            {
                end_game_message = "You have lost :(";
            }
            else
            {
                end_game_message = "You have won :)";
            }
            gtk_label_set_text(GTK_LABEL(end_game_server_gui), g_strdup_printf("Player %d has lost.\n%s", player_server_gui == 2 ? 2 : 1, end_game_message));
            // Add an exit button
            GtkWidget *exit_button = gtk_button_new_with_label("Exit");
            gtk_widget_add_css_class(exit_button, "btn_exit");
            g_signal_connect(exit_button, "clicked", G_CALLBACK(exit_game_server_gui), NULL);
            gtk_grid_attach(GTK_GRID(main_div_server_gui), exit_button, 1, 4, 1, 1);
        }

        // Update the player label
        player_server_gui = (player_server_gui % 2) + 1;
        gtk_label_set_text(GTK_LABEL(player_label_server_gui), g_strdup_printf("Player %d (Server)", player_server_gui));
        gtk_widget_add_css_class(player_label_server_gui, "player_label");

        // Enable the buttons after the local player's turn
        set_buttons_state_server_gui(TRUE);
    }

    return NULL;
}

/**
 * @brief Function to start the server
 *
 * @param port : The port number
 * @return int : The status of the server
 */
int network_server_gui(unsigned short port)
{
    port_server_gui = port;
    return launch_gui_server_gui();
}