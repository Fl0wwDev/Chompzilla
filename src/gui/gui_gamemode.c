#include <stdio.h>
#include <limits.h>
#include <arpa/inet.h>
#include "../inc/const.h"
#include "gui_gamemode.h"
#include "../game_logic/game_logic.h"
#include "../game.h"
#include <gtk/gtk.h>
#include "../server/server.h"
#include "client_gui.h"
#include "server_gui.h"

// Global variables
GtkApplication* app_gamemode;
GtkWidget* window_gamemode;
GtkWidget* dialog_wait;
unsigned short port;

/**
 * @brief Function to destroy the waiting window
 *
 * @return void
 */
void destroy_window_gamemode_wait() {
    if (dialog_wait != NULL && gtk_widget_get_visible(dialog_wait)) {
        gtk_window_destroy(GTK_WINDOW(dialog_wait));
        dialog_wait = NULL;
    }
}

/**
 * @brief Function to start the server from the GUI
 *
 * @param port The port to start the server on
 * @return void
 */
void start_button_server_gui(GtkWidget* button, gpointer user_data) 
{
    // We get the port from the entry
    GtkWidget* grid = GTK_WIDGET(gtk_widget_get_parent(button));
    GtkWidget* entry_port = GTK_WIDGET(user_data);

    if (GTK_IS_ENTRY(entry_port)) {
        const char* port_str = gtk_editable_get_text(GTK_EDITABLE(entry_port));
        // We verify that the port can be converted to an unsigned short
        printf("STR LEN: %ld\n", strlen(port_str));
        if (port_str == NULL || strlen(port_str) == 0 || strlen(port_str) > 5 || atoi(port_str) > USHRT_MAX) {
            printf("Port number is empty.\n");
            // We add an error message to the grid
            GtkWidget* label_error = gtk_label_new("Please enter a port number.");
            gtk_widget_set_margin_bottom(label_error, 10);
            gtk_widget_set_margin_top(label_error, 10);
            gtk_widget_set_margin_start(label_error, 10);
            gtk_widget_set_margin_end(label_error, 10);
            gtk_grid_attach(GTK_GRID(grid), label_error, 0, 3, 2, 1);
            return;
        }
        else {
            port = (unsigned short)atoi(port_str);
        }
    }

    if (port == 0 || port > 65535 || port < 1024) {
        printf("Invalid port number.\n");
        // We add an error message to the grid
        GtkWidget* label_error = gtk_label_new("Please enter a valid port number.");
        gtk_widget_set_margin_bottom(label_error, 10);
        gtk_widget_set_margin_top(label_error, 10);
        gtk_widget_set_margin_start(label_error, 10);
        gtk_widget_set_margin_end(label_error, 10);
        gtk_grid_attach(GTK_GRID(grid), label_error, 0, 3, 2, 1);
        return;
    }

    // We destroy the window
    gtk_window_destroy(GTK_WINDOW(gtk_widget_get_parent(grid)));

    // We start the server
    network_server_gui(port);
}

/**
 * @brief Function to destroy the gamemode window
 *
 * @return void
 */
void destroy_window_gamemode() {
    if (window_gamemode != NULL && gtk_widget_get_visible(window_gamemode)) {
        gtk_window_destroy(GTK_WINDOW(window_gamemode));
        window_gamemode = NULL; // Ne pas oublier de réinitialiser le pointeur pour éviter d'essayer de détruire la fenêtre à nouveau
    }
}

/**
 * @brief Function to play a local game with the GUI
 *
 * @param button The button clicked
 * @param user_data The data passed to the function
 * @return void
 */
void play_local_game_gui_button(GtkWidget* button, gpointer user_data) 
{
    // We destroy the window
    destroy_window_gamemode();

    // We launch the GUI
    play_local_game_gui();
}

/**
 * @brief Function to play a local game with the GUI
 * 
 * @param button The button clicked
 * @param user_data The data passed to the function
 * @return void
 */
void start_button_client_gui(GtkWidget* button, gpointer user_data)
{
    printf("Starting the client.\n");
    // We get the ip and port from the entries
    GtkWidget* grid = GTK_WIDGET(gtk_widget_get_parent(button));
    GtkWidget* entry_ip = GTK_WIDGET(((IpPortGtk*)user_data)->entry_ip);
    GtkWidget* entry_port = GTK_WIDGET(((IpPortGtk*)user_data)->entry_port);
    
    printf("IP: %p\n", entry_ip);
    printf("Port: %p\n", entry_port);
    
    if (GTK_IS_ENTRY(entry_ip) && GTK_IS_ENTRY(entry_port)) {
        const char* ip = gtk_editable_get_text(GTK_EDITABLE(entry_ip));
        printf("IP: %s\n", ip);
        const char* port_str = gtk_editable_get_text(GTK_EDITABLE(entry_port));
        printf("Port: %s\n", port_str);

        // We verify that the port can be converted to an unsigned short
        if (port_str == NULL || strlen(port_str) == 0 || strlen(port_str) > 5 || atoi(port_str) > USHRT_MAX) {
            printf("Port number is empty.\n");
            // We add an error message to the grid
            GtkWidget* label_error = gtk_label_new("Please enter a port number.");
            gtk_widget_set_margin_bottom(label_error, 10);
            gtk_widget_set_margin_top(label_error, 10);
            gtk_widget_set_margin_start(label_error, 10);
            gtk_widget_set_margin_end(label_error, 10);
            gtk_grid_attach(GTK_GRID(grid), label_error, 0, 4, 2, 1);
            return;
        }
        else {
            port = (unsigned short)atoi(port_str);
        }

        if (port == 0 || port > 65535 || port < 1024) {
            printf("Invalid port number.\n");
            // We add an error message to the grid
            GtkWidget* label_error = gtk_label_new("Please enter a valid port number.");
            gtk_widget_set_margin_bottom(label_error, 10);
            gtk_widget_set_margin_top(label_error, 10);
            gtk_widget_set_margin_start(label_error, 10);
            gtk_widget_set_margin_end(label_error, 10);
            gtk_grid_attach(GTK_GRID(grid), label_error, 0, 4, 2, 1);
            return;
        }

        // We verify that the ip is correct
        struct sockaddr_in sa;
        int result = inet_pton(AF_INET, ip, &(sa.sin_addr));
        if (result != 1) {
            printf("Invalid IP address.\n");
            // We add an error message to the grid
            GtkWidget* label_error = gtk_label_new("Please enter a valid IP address.");
            gtk_widget_set_margin_bottom(label_error, 10);
            gtk_widget_set_margin_top(label_error, 10);
            gtk_widget_set_margin_start(label_error, 10);
            gtk_widget_set_margin_end(label_error, 10);
            gtk_grid_attach(GTK_GRID(grid), label_error, 0, 4, 2, 1);
            return;
        }

        // We destroy the window
        gtk_window_destroy(GTK_WINDOW(gtk_widget_get_parent(grid)));

        // We start the client
        network_client_gui(ip, port);
    }
}

/**
 * @brief Function to host a server with the GUI
 *
 * @param button The button clicked
 * @param user_data The data passed to the function
 * @return void
 */
void host_server_gui_button(GtkWidget* button, gpointer user_data) 
{
    // We open a window to ask for the port
    GtkWidget* dialog_port = gtk_window_new();

    // We set the title of the window
    gtk_window_set_title(GTK_WINDOW(dialog_port), "Enter the port");

    // We create a grid to add the elements
    GtkWidget* grid_port = gtk_grid_new();

    // We add a container for the label + entry of the port
    GtkWidget* container_port = gtk_grid_new();

    // We add a label for the port in the grid
    GtkWidget* label_port = gtk_label_new("Port:");
    gtk_grid_attach(GTK_GRID(container_port), label_port, 0, 1, 1, 1);

    // We add a text entry for the port in the grid
    GtkWidget* entry_port = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(container_port), entry_port, 1, 1, 1, 1);
    gtk_widget_set_margin_start(entry_port, 10);

    // We add a margin to the container
    gtk_widget_set_margin_bottom(container_port, 10);
    gtk_widget_set_margin_top(container_port, 10);
    gtk_widget_set_margin_start(container_port, 10);
    gtk_widget_set_margin_end(container_port, 10);

    // We add the container to the grid
    gtk_grid_attach(GTK_GRID(grid_port), container_port, 0, 1, 2, 1);

    // We add a button to connect to the server
    GtkWidget* button_connect = gtk_button_new_with_label("Connect");
    gtk_widget_set_margin_bottom(button_connect, 10);
    gtk_widget_set_margin_top(button_connect, 10);
    gtk_widget_set_margin_start(button_connect, 10);
    gtk_widget_set_margin_end(button_connect, 10);
    gtk_grid_attach(GTK_GRID(grid_port), button_connect, 0, 2, 2, 1);
    g_signal_connect(button_connect, "clicked", G_CALLBACK(start_button_server_gui), entry_port);

    // We add the grid to the window
    gtk_window_set_child(GTK_WINDOW(dialog_port), grid_port);

    // We show the window
    gtk_window_present(GTK_WINDOW(dialog_port));
}

/**
 * @brief Function to play a network game with the GUI as a client
 * 
 * @param button The button clicked
 * @param user_data The data passed to the function
 * @return void
 */
void play_network_game_gui_button(GtkWidget* button, gpointer user_data)
{
    // We open a dialog to ask for the ip and port
    GtkWidget* dialog_ip_port = gtk_window_new();

    // We set the title of the window
    gtk_window_set_title(GTK_WINDOW(dialog_ip_port), "Enter the IP and port");

    // We create a grid to add the elements
    GtkWidget* grid_ip_port = gtk_grid_new();

    // We add a container for the label + entry of the ip
    GtkWidget* container_ip = gtk_grid_new();

    // We add a label for the ip in the grid
    GtkWidget* label_ip = gtk_label_new("IP address:");
    gtk_grid_attach(GTK_GRID(container_ip), label_ip, 0, 1, 1, 1);
    
    // We add a text entry for the ip in the grid
    GtkWidget* entry_ip = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(container_ip), entry_ip, 1, 1, 1, 1);
    gtk_widget_set_margin_start(entry_ip, 10);

    // We add a margin to the container
    gtk_widget_set_margin_bottom(container_ip, 10);
    gtk_widget_set_margin_top(container_ip, 10);
    gtk_widget_set_margin_start(container_ip, 10);
    gtk_widget_set_margin_end(container_ip, 10);

    // We add the container to the grid
    gtk_grid_attach(GTK_GRID(grid_ip_port), container_ip, 0, 1, 2, 1);

    // We add a container for the label + entry of the port
    GtkWidget* container_port = gtk_grid_new();

    // We add a label for the port in the grid
    GtkWidget* label_port = gtk_label_new("Port:");
    gtk_grid_attach(GTK_GRID(container_port), label_port, 0, 1, 1, 1);

    // We add a text entry for the port in the grid
    GtkWidget* entry_port = gtk_entry_new();
    gtk_grid_attach(GTK_GRID(container_port), entry_port, 1, 1, 1, 1);
    gtk_widget_set_margin_start(entry_port, 10);

    // We add a margin to the container
    gtk_widget_set_margin_bottom(container_port, 10);
    gtk_widget_set_margin_top(container_port, 10);
    gtk_widget_set_margin_start(container_port, 10);
    gtk_widget_set_margin_end(container_port, 10);

    // We add the container to the grid
    gtk_grid_attach(GTK_GRID(grid_ip_port), container_port, 0, 2, 2, 1);

    // We add a button to connect to the server
    GtkWidget* button_connect = gtk_button_new_with_label("Connect");
    gtk_widget_set_margin_bottom(button_connect, 10);
    gtk_widget_set_margin_top(button_connect, 10);
    gtk_widget_set_margin_start(button_connect, 10);
    gtk_widget_set_margin_end(button_connect, 10);
    gtk_grid_attach(GTK_GRID(grid_ip_port), button_connect, 0, 3, 2, 1);

    // We create a structure to pass the ip and port to the callback
    IpPortGtk* ip_port = malloc(sizeof(IpPortGtk));

    // We add the ip and port to the structure
    ip_port->entry_ip = entry_ip;
    ip_port->entry_port = entry_port;

    g_signal_connect(button_connect, "clicked", G_CALLBACK(start_button_client_gui), ip_port);

    // We add the grid to the window
    gtk_window_set_child(GTK_WINDOW(dialog_ip_port), grid_ip_port);

    // We show the window
    gtk_window_present(GTK_WINDOW(dialog_ip_port));
}

/**
 * @brief Function to play a game against the IA with the GUI
 *
 * @param button The button clicked
 * @param user_data The data passed to the function
 * @return void
 */
static void play_vs_ia_gui_button(GtkWidget* button, gpointer user_data) {
    // We destroy the window
    destroy_window_gamemode();

    // We launch the GUI (not implemented yet)
    // play_vs_ia_gui();
}

/**
 * @brief The main window of the application
 *
 * @param app The application
 * @param user_data The data passed to the function
 * @return void
 */
static void active_gui_gamemode(GtkApplication* app, gpointer user_data) {
    // Creation of the window
    window_gamemode = gtk_application_window_new(app_gamemode);
    char* css_gamemode = ".title_label{font-size: 20px;} .btn_gamemode{margin-bottom: 10px;}";

    // Setting the parameters of the window (title and size)
    gtk_window_set_title(GTK_WINDOW(window_gamemode), NAME);
    gtk_window_set_default_size(GTK_WINDOW(window_gamemode), WINDOW_GAMEMODE_WIDTH, WINDOW_GAMEMODE_HEIGHT);

    // Load the CSS
    GtkCssProvider* provider_gamemode = gtk_css_provider_new();
    printf("GTK version: %d.%d.%d\n", GTK_MAJOR_VERSION, GTK_MINOR_VERSION, GTK_MICRO_VERSION);

    // Check the GTK version to use the appropriate function
#if GTK_CHECK_VERSION(4, 12, 0)
    // user version >= 4.12.0
    gtk_css_provider_load_from_string(provider_gamemode, css_gamemode);
#else
    // version < 4.12.0
    gtk_css_provider_load_from_data(provider_gamemode, css_gamemode, -1);
#endif

    gtk_style_context_add_provider_for_display(gdk_display_get_default(), GTK_STYLE_PROVIDER(provider_gamemode), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    // We add a css class to the window
    gtk_widget_add_css_class(window_gamemode, "window_gamemode");

    // We create a main_div
    GtkWidget* main_div_gamemode = gtk_grid_new();

    // Center the main_div in the window
    gtk_widget_set_halign(main_div_gamemode, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(main_div_gamemode, GTK_ALIGN_CENTER);

    // We create a label to display the title
    GtkWidget* title_label = gtk_label_new("Choose the game mode");
    gtk_widget_add_css_class(title_label, "title_label");

    // We add the title_label to the main_div
    gtk_grid_attach(GTK_GRID(main_div_gamemode), title_label, 0, 0, 1, 1);

    // Add a margin under the label
    gtk_widget_set_margin_bottom(title_label, 10);

    // We create a button to play local multiplayer
    GtkWidget* local_button = gtk_button_new_with_label("Local Multiplayer");
    gtk_widget_add_css_class(local_button, "btn_gamemode");
    g_signal_connect(local_button, "clicked", G_CALLBACK(play_local_game_gui_button), NULL);

    // We add the button to the main_div
    gtk_grid_attach(GTK_GRID(main_div_gamemode), local_button, 0, 1, 1, 1);

    // We create a button to host a server
    GtkWidget* network_button = gtk_button_new_with_label("Host a server");
    gtk_widget_add_css_class(network_button, "btn_gamemode");
    g_signal_connect(network_button, "clicked", G_CALLBACK(host_server_gui_button), NULL);

    // We add the button to the main_div
    gtk_grid_attach(GTK_GRID(main_div_gamemode), network_button, 0, 2, 1, 1);

    // We create a button to connect to a server
    GtkWidget* connect_button = gtk_button_new_with_label("Connect to a server");
    gtk_widget_add_css_class(connect_button, "btn_gamemode");
    g_signal_connect(connect_button, "clicked", G_CALLBACK(play_network_game_gui_button), NULL);

    // We add the button to the main_div
    gtk_grid_attach(GTK_GRID(main_div_gamemode), connect_button, 0, 3, 1, 1);

    // Add the main_div to the window
    gtk_window_set_child(GTK_WINDOW(window_gamemode), main_div_gamemode);

    // Showing the window
    gtk_window_present(GTK_WINDOW(window_gamemode));
}

/**
 * @brief The main function of the application
 *
 * @param argc The number of arguments
 * @param argv The arguments
 * @return int The return code
 */
int launch_gui_gamemode(int argc, char** argv) {
    int status;

    app_gamemode = gtk_application_new("org.chompzilla.gamemenu", 0);
    g_signal_connect(app_gamemode, "activate", G_CALLBACK(active_gui_gamemode), NULL);
    status = g_application_run(G_APPLICATION(app_gamemode), argc, argv);
    g_object_unref(app_gamemode);

    return status;
}
