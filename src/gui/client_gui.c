#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include "client_gui.h"
#include "../inc/const.h"

GtkApplication *app_client_gui;
GtkWidget *window_client_gui;
GtkWidget *main_div_client_gui;
GtkWidget *player_label_client_gui;
GtkWidget *error_label_client_gui;
GtkWidget *end_game_client_gui;
GtkWidget *table_client_gui;
int player_client_gui = 1;
int buttons_deleted_client_gui = 0;
// 0: server turn, 1: client turn
int is_local_player_turn_client_gui = 1;
// Global socket variable
int sock_client_gui = 0;
// Mutex to protect the turn variable
pthread_mutex_t turn_mutex = PTHREAD_MUTEX_INITIALIZER;
const char *server_ip_client_gui;
unsigned short port_client_gui;

/**
 * @brief Function to destroy the window_client_gui
 * @return void
 */
void destroy_window_client_gui()
{
    if (window_client_gui != NULL)
    {
        printf("Destroying window...\n");
        gtk_window_destroy(GTK_WINDOW(window_client_gui));
    }
}

/**
 * @brief Function to quit the game
 * @return void
 */
void exit_game_client_gui(GtkWidget *button, gpointer user_data)
{
    printf("Exiting game...\n");
    destroy_window_client_gui();
}

/**
 * @brief Function to reset the game state
 * This function will reset all the global variables and the game logic state.
 * @return void
 */
void reset_game_state_client_gui()
{
    printf("Resetting game state...\n");
    player_client_gui = 1;
    buttons_deleted_client_gui = 0; // Reset the counter
    pthread_mutex_lock(&turn_mutex);
    is_local_player_turn_client_gui = 1; // Reset the turn
    pthread_mutex_unlock(&turn_mutex);
}

/**
 * @brief Function to update the label text
 *
 * @param data The data to update
 * @return gboolean
 */
gboolean update_label_text_client_gui(gpointer data)
{
    const char *text = (const char *)data;
    gtk_label_set_text(GTK_LABEL(player_label_client_gui), text);
    g_free(data);
    return FALSE;
}

/**
 * @brief Function to update the error label text
 *
 * @param data The data to update
 * @return gboolean
 */
gboolean update_error_label_text_client_gui(gpointer data)
{
    const char *text = (const char *)data;
    gtk_label_set_text(GTK_LABEL(error_label_client_gui), text);
    g_free(data);
    return FALSE;
}

/**
 * @brief Function to send the coordinates to the server
 *
 * @param arg The argument to pass to the function (coordinates)
 * @return void*
 */
void *start_client_send_client_gui(void *arg)
{
    char *button_coordinate = (char *)arg;

    printf("Sending coordinates to server: %s\n", button_coordinate);
    ssize_t bytes_sent = send(sock_client_gui, button_coordinate, strlen(button_coordinate), 0);
    if (bytes_sent < 0)
    {
        printf("Failed to send coordinates to server.\n");
    }
    else
    {
        printf("Successfully sent %ld bytes to server.\n", bytes_sent);
    }

    return NULL;
}

/**
 * @brief The function to delete a button and the linked buttons
 * This function is called when a button is clicked
 * This function also calls the client to send the coordinates of the block to get the coordinates to delete
 * @param *button : The button clicked
 * @param user_data : The data passed to the function
 */
static void del_button_client_gui(GtkWidget *button, gpointer user_data)
{
    pthread_mutex_lock(&turn_mutex);
    if (!is_local_player_turn_client_gui)
    {
        printf("It's not your turn.\n");
        pthread_mutex_unlock(&turn_mutex);
        return;
    }
    pthread_mutex_unlock(&turn_mutex);

    printf("Deleting button locally...\n");
    const char *button_coordinate = gtk_widget_get_name(button);
    printf("Button clicked: %s\n", button_coordinate);

    // Delete the button locally
    int x = button_coordinate[0] - 'A';
    int y = button_coordinate[1] - '1';

    if (!GTK_IS_GRID(table_client_gui))
    {
        printf("Error: The table widget is not a GtkGrid.\n");
        return;
    }

    int new_buttons_to_delete = 0;

    // First, count only the new deletable buttons
    for (int i = x; i < GAME_X; i++)
    {
        for (int j = y; j < GAME_Y; j++)
        {
            GtkWidget *btn = gtk_grid_get_child_at(GTK_GRID(table_client_gui), i + 1, j + 1);
            if (btn && !gtk_widget_has_css_class(btn, "btn_del"))
            {
                new_buttons_to_delete++;
                printf("Button at [%d, %d] can be deleted (total new: %d)\n", i, j, new_buttons_to_delete);

                if (new_buttons_to_delete > 5)
                {
                    printf("new_buttons_to_delete: %d\n", new_buttons_to_delete);
                    printf("Cannot delete more than 5 buttons.\n");
                    // Add the error message to the error label
                    char *error_message = g_strdup("Cannot delete more than 5 buttons.");
                    g_idle_add(update_error_label_text_client_gui, error_message);
                    return;
                }
                else
                {
                    // Clear the error message
                    char *error_message = g_strdup("");
                    g_idle_add(update_error_label_text_client_gui, error_message);
                }
            }
            else if (btn && gtk_widget_has_css_class(btn, "btn_del"))
            {
                printf("Button at [%d, %d] already deleted or not found. Ignoring it.\n", i, j);
            }
        }
    }

    printf("Potential new buttons to delete in this move: %d\n", new_buttons_to_delete);

    // Proceed with deletion only if the number of new buttons is within the limit
    for (int i = x; i < GAME_X; i++)
    {
        for (int j = y; j < GAME_Y; j++)
        {
            GtkWidget *btn = gtk_grid_get_child_at(GTK_GRID(table_client_gui), i + 1, j + 1);
            if (btn && !gtk_widget_has_css_class(btn, "btn_del"))
            {
                gtk_widget_add_css_class(btn, "btn_del");
                g_signal_handlers_disconnect_by_func(btn, G_CALLBACK(del_button_client_gui), NULL);
                buttons_deleted_client_gui++;
                printf("Deleted button locally at [%d, %d]. Total deleted now: %d\n", i, j, buttons_deleted_client_gui);
                if (buttons_deleted_client_gui == ROWS * COLS)
                {
                    char *end_game_message = "";
                    if (player_client_gui == 1)
                    {
                        end_game_message = "You have lost :(";
                    }
                    else
                    {
                        end_game_message = "You have won :)";
                    }

                    gtk_label_set_text(GTK_LABEL(end_game_client_gui), g_strdup_printf("Player %d has lost.\n%s", player_client_gui, end_game_message));
                    // Add an exit button
                    GtkWidget *exit_button = gtk_button_new_with_label("Exit");
                    gtk_widget_add_css_class(exit_button, "btn_exit");
                    g_signal_connect(exit_button, "clicked", G_CALLBACK(exit_game_client_gui), NULL);
                    gtk_grid_attach(GTK_GRID(main_div_client_gui), exit_button, 1, 4, 1, 1);
                }
            }
        }
    }

    // Once the button is deleted, send the coordinates to the server
    pthread_t send_thread;
    pthread_create(&send_thread, NULL, start_client_send_client_gui, (void *)button_coordinate);
    pthread_detach(send_thread);

    // Change the turn to the server
    pthread_mutex_lock(&turn_mutex);
    is_local_player_turn_client_gui = 0;
    pthread_mutex_unlock(&turn_mutex);

    // Update of the user interface from the main thread
    char *label_text = g_strdup("Waiting for server...");
    g_idle_add(update_label_text_client_gui, label_text);
}

/**
 * @brief The main window of the application
 * @param *app_client_gui : The application
 * @param user_data : The data passed to the function
 * @return void
 */
static void active_gui_client_gui(GtkApplication *app_client_gui, gpointer user_data)
{
    printf("Activating GUI...\n");
    GtkWidget *button;
    GtkWidget *center_box;
    char *css = ".btn {background: #ffc300; margin: 1px;} .btn:hover{border: 2px solid #D72638;} .window {background: #001d3d;} .player_label {font-size: 20px; font-weight: bold;} .error_label {color: red; font-size: 18px; font-weight: bold;} .btn_exit {background: #00916E; color: black;} .player_label{color: white;} .coords{color: white;} .btn_del{opacity: 0; transition: opacity 0.2s;}";
    char *coordinate;

    window_client_gui = gtk_application_window_new(app_client_gui);
    gtk_window_set_title(GTK_WINDOW(window_client_gui), NAME);
    gtk_window_set_default_size(GTK_WINDOW(window_client_gui), WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_widget_add_css_class(window_client_gui, "window");

    main_div_client_gui = gtk_grid_new();
    gtk_widget_set_halign(main_div_client_gui, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_div_client_gui, GTK_ALIGN_CENTER);
    gtk_grid_set_column_homogeneous(GTK_GRID(main_div_client_gui), TRUE);
    gtk_grid_set_column_spacing(GTK_GRID(main_div_client_gui), 10);

    GtkCssProvider *provider = gtk_css_provider_new();
#if GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(provider, css);
#else
    gtk_css_provider_load_from_data(provider, css, -1);
#endif
    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    player_label_client_gui = gtk_label_new(g_strdup_printf("Player %d", 1));
    gtk_grid_attach(GTK_GRID(main_div_client_gui), player_label_client_gui, 1, 0, 1, 1);
    gtk_widget_add_css_class(player_label_client_gui, "player_label");
    gtk_widget_set_margin_bottom(player_label_client_gui, 10);

    // Initialisation of the game board
    table_client_gui = gtk_grid_new();
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
        gtk_grid_attach(GTK_GRID(table_client_gui), coord, i + 1, 0, 1, 1);
        gtk_widget_add_css_class(coord, "coords");
        gtk_widget_set_margin_bottom(coord, 10);

        for (int j = 0; j < GAME_Y; j++)
        {
            if (i == 0)
            {
                coord = gtk_label_new(g_strdup_printf("%d", j + 1));
                gtk_grid_attach(GTK_GRID(table_client_gui), coord, 0, j + 1, 1, 1);
                gtk_widget_add_css_class(coord, "coords");
                gtk_widget_set_margin_end(coord, 10);
            }
            char *full_coordinate = g_strconcat(coordinate, g_strdup_printf("%d", j + 1), NULL);
            button = gtk_button_new_with_label(" ");
            gtk_widget_set_size_request(button, BUTTON_SIZE, BUTTON_SIZE);
            gtk_widget_add_css_class(button, "btn");
            gtk_widget_set_name(button, full_coordinate);
            g_signal_connect(button, "clicked", G_CALLBACK(del_button_client_gui), NULL);
            gtk_grid_attach(GTK_GRID(table_client_gui), button, i + 1, j + 1, 1, 1);
            g_free(full_coordinate);
        }
    }

    center_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_halign(center_box, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(center_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(center_box), table_client_gui);

    // We add the error label under the game board
    error_label_client_gui = gtk_label_new("");
    gtk_widget_add_css_class(error_label_client_gui, "error_label");
    gtk_grid_attach(GTK_GRID(main_div_client_gui), error_label_client_gui, 1, 2, 1, 1);
    gtk_widget_set_margin_top(error_label_client_gui, 10);

    // We add the end game label under the error label
    end_game_client_gui = gtk_label_new("");
    gtk_widget_add_css_class(end_game_client_gui, "player_label");
    gtk_grid_attach(GTK_GRID(main_div_client_gui), end_game_client_gui, 1, 3, 1, 1);
    gtk_widget_set_margin_top(end_game_client_gui, 10);
    gtk_widget_set_margin_bottom(end_game_client_gui, 10);

    gtk_grid_attach(GTK_GRID(main_div_client_gui), center_box, 0, 1, 3, 1);
    gtk_window_set_child(GTK_WINDOW(window_client_gui), main_div_client_gui);
    gtk_window_present(GTK_WINDOW(window_client_gui));

    // Disable the buttons on startup
    pthread_mutex_lock(&turn_mutex);
    is_local_player_turn_client_gui = 1;
    pthread_mutex_unlock(&turn_mutex);
}

/**
 * @brief The function to receive the coordinates from the server
 *
 * @param arg The argument to pass to the function (coordinates)
 * @return void*
 */
void *start_client_receive_client_gui(void *arg)
{
    char buffer[BUFFER_SIZE] = {0};

    while (1)
    {
        int bytes_read = read(sock_client_gui, buffer, BUFFER_SIZE);
        if (bytes_read > 0)
        {
            buffer[bytes_read] = '\0';
            printf("Received coordinates: %s\n", buffer);

            // Process the coordinates and update the UI
            int x = buffer[0] - 'A';
            int y = buffer[1] - '1';

            if (!GTK_IS_GRID(table_client_gui))
            {
                printf("Error: The table widget is not a GtkGrid.\n");
                continue;
            }

            int new_buttons_to_delete = 0;

            // First, count only the new deletable buttons
            for (int i = x; i < GAME_X; i++)
            {
                for (int j = y; j < GAME_Y; j++)
                {
                    GtkWidget *btn = gtk_grid_get_child_at(GTK_GRID(table_client_gui), i + 1, j + 1);
                    if (btn && !gtk_widget_has_css_class(btn, "btn_del"))
                    {
                        new_buttons_to_delete++;
                    }
                }
            }

            // Print potential new buttons to delete in this move
            printf("Potential new buttons to delete in this move: %d\n", new_buttons_to_delete);

            // We proceed with deletion only if the number of new buttons is within the limit
            for (int i = x; i < GAME_X; i++)
            {
                for (int j = y; j < GAME_Y; j++)
                {
                    GtkWidget *btn = gtk_grid_get_child_at(GTK_GRID(table_client_gui), i + 1, j + 1);
                    if (btn && !gtk_widget_has_css_class(btn, "btn_del"))
                    {
                        gtk_widget_add_css_class(btn, "btn_del");
                        g_signal_handlers_disconnect_by_func(btn, G_CALLBACK(del_button_client_gui), NULL);
                        buttons_deleted_client_gui++;
                        printf("Deleted button locally at [%d, %d]. Total deleted now: %d\n", i, j, buttons_deleted_client_gui);
                    }
                }
            }

            // We verify that we have not reached the end of the game
            if (buttons_deleted_client_gui == ROWS * COLS)
            {
                // We check if the current player has win or lose
                char* end_game_message = "";
                if (player_client_gui == 2){
                    end_game_message = "You have lost :(";
                } else {
                    end_game_message = "You have won :)";
                }
                gtk_label_set_text(GTK_LABEL(end_game_client_gui), g_strdup_printf("Player %d has lost.\n%s", player_client_gui == 1 ? 2 : 1, end_game_message));
                // Add an exit button
                GtkWidget *exit_button = gtk_button_new_with_label("Exit");
                gtk_widget_add_css_class(exit_button, "btn_exit");
                g_signal_connect(exit_button, "clicked", G_CALLBACK(exit_game_client_gui), NULL);
                gtk_grid_attach(GTK_GRID(main_div_client_gui), exit_button, 1, 4, 1, 1);
            }

            // Switch turn to client
            pthread_mutex_lock(&turn_mutex);
            is_local_player_turn_client_gui = 1;
            pthread_mutex_unlock(&turn_mutex);

            // Update the user interface from the main thread
            char *label_text = g_strdup_printf("Player %d", player_client_gui);
            g_idle_add(update_label_text_client_gui, label_text);
        }
        else if (bytes_read == 0)
        {
            // The server closed the connection
            printf("Server closed the connection.\n");
            break;
        }
        else
        {
            perror("Error reading from server");
            close(sock_client_gui);
            break;
        }
    }
    return NULL;
}

/**
 * @brief The main function of the application
 *
 * @return int : The status of the application
 */
int launch_gui_client_gui()
{
    printf("Launching GUI...\n");
    int status;

    // Create the client socket
    if ((sock_client_gui = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port_client_gui);

    if (inet_pton(AF_INET, server_ip_client_gui, &serv_addr.sin_addr) <= 0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock_client_gui, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, start_client_receive_client_gui, NULL);
    pthread_detach(recv_thread);

    app_client_gui = gtk_application_new("org.chompzilla.chompclient", 0);
    g_signal_connect(app_client_gui, "activate", G_CALLBACK(active_gui_client_gui), NULL);
    status = g_application_run(G_APPLICATION(app_client_gui), 0, NULL);
    g_object_unref(app_client_gui);

    return status;
}

/**
 * @brief Function to launch the client in GUI mode on a specific IP address and port
 *
 * @param server_ip The IP address of the server
 * @param port The port number
 */
int network_client_gui(const char *server_ip, unsigned short port)
{
    server_ip_client_gui = server_ip;
    port_client_gui = port;
    return launch_gui_client_gui();
}