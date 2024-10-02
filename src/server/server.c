#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include "server.h"
#include "../inc/const.h"
#include "../game_logic/game_logic.h"
#include "../gui/gui_gamemode.h"
#include "../ia/ia.h"

// Global var
int current_player = 1;
int server_ready = 0;
int server_fd = -1;
int new_socket = -1;

pthread_mutex_t server_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t server_cond = PTHREAD_COND_INITIALIZER;

/**
 * @brief Function to handle the client move
 * This function updates the GUI with the client move
 *
 * @param coordinates The coordinates of the client move
 * @return void
 */
void handle_client_move(const char *coordinates)
{
    // g_idle_add is used to call the function from the GTK thread
    g_idle_add(update_gui_from_server, g_strdup(coordinates));
}

/**
 * @brief Function to close the sockets
 *
 * @return void
 */
void close_sockets()
{
    if (new_socket != -1)
    {
        close(new_socket);
        new_socket = -1;
    }
    if (server_fd != -1)
    {
        close(server_fd);
        server_fd = -1;
    }
    printf("Sockets closed.\n");
}

/**
 * @brief Function to handle the interrupt signal (CTRL+C)
 *
 * @param sig The signal number
 * @return void
 */
void handle_sigint(int sig)
{
    printf("\nInterrupt signal (%d) received. Closing sockets...\n", sig);
    close_sockets();
    exit(0);
}

/**
 * @brief Function to manage the network server
 *
 * @param port The port number
 * @return void
 */
void play_network_server(unsigned short port)
{
    struct sockaddr_in address, client_address;
    char buffer[1024] = {0};
    int addrlen = sizeof(address);
    int client_addrlen = sizeof(client_address);
    int client_connected = 0;

    // Buffer to store the destroyed squares
    char destroyed[ROWS * COLS][3];
    int destroyed_count = 0;

    printf("Creating socket...\n");
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Configuration of the socket to reuse the address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close_sockets();
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to the port and address
    printf("Binding socket...\n");
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close_sockets();
        exit(EXIT_FAILURE);
    }

    // Listen for 1 connection
    printf("Listening for connections...\n");
    if (listen(server_fd, 1) < 0)
    {
        perror("Listen failed");
        close_sockets();
        exit(EXIT_FAILURE);
    }

    printf("Waiting for a player to connect...\n");

    // Send a signal to the GUI to indicate that the server is ready
    pthread_mutex_lock(&server_mutex);
    server_ready = 1;
    pthread_cond_signal(&server_cond);
    pthread_mutex_unlock(&server_mutex);

    while (1)
    {
        if (client_connected)
        {
            printf("Server full: No more connections allowed.\n");
            sleep(1);
            continue;
        }

        printf("Accepting connection...\n");
        new_socket = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t *)&client_addrlen);

        if (new_socket < 0)
        {
            perror("Accept failed");
            continue;
        }

        client_connected = 1;

        char *client_ip = inet_ntoa(client_address.sin_addr);
        printf("Player connected! IP: %s\n", client_ip);
        printf("\n");

        // Lauch the game GUI from the GTK thread
        printf("Launching game GUI...\n");
        g_idle_add((GSourceFunc)launch_game_gui_on_client_connect, NULL);

        printf("Initializing game board...\n");
        init_board();
        display_board();

        while (client_connected)
        {
            char col;
            int row;
            int valid_move = 0;
            int bytes_read;
            char *server_destroyed[ROWS * COLS];
            int server_destroyed_count = 0;

            // Client's turn
            if (current_player == 1)
            {
                // Allocate memory and copy the board to server_destroyed to get the destroyed before the client plays
                for (int i = 0; i < ROWS * COLS; i++)
                {
                    server_destroyed[i] = malloc(3 * sizeof(char));
                    if (server_destroyed[i] == NULL)
                    {
                        // Handle memory allocation failure
                        fprintf(stderr, "Memory allocation failed for server_destroyed[%d]\n", i);
                        exit(EXIT_FAILURE);
                    }
                    strcpy(server_destroyed[i], destroyed[i]);
                }
                server_destroyed_count = destroyed_count;
                printf("Waiting for the opponent's move...\n");
                valid_move = 0;
                while (!valid_move)
                {
                    memset(buffer, 0, sizeof(buffer));
                    bytes_read = read(new_socket, buffer, sizeof(buffer));

                    if (bytes_read == 0 || bytes_read == -1)
                    {
                        printf("Client disconnected.\n");
                        close(new_socket);
                        new_socket = -1;
                        client_connected = 0;
                        break;
                    }

                    sscanf(buffer, "%c%d", &col, &row);
                    printf("Opponent chose: %c%d\n", col, row);
                    valid_move = make_move(col, row, destroyed, &destroyed_count, false);
                    if (!valid_move)
                    {
                        printf("Invalid move received from opponent. Waiting for a new move...\n");
                    }
                }

                if (!client_connected)
                    break;

                display_board();
                printf("Destroyed squares: ");
                for (int i = 0; i < destroyed_count; i++)
                {
                    printf("%s ", destroyed[i]);
                }
                printf("\n");

                // Update the GUI with the opponent's move
                char *move_to_send = strdup(buffer);
                g_idle_add(update_gui_from_server, (gpointer)move_to_send);

                if (is_game_over())
                {
                    // We verify if A1 is stored in the destroyed array
                    bool looser = false;
                    for (int i = 0; i < server_destroyed_count; i++)
                    {
                        if (strcmp(server_destroyed[i], "A1") == 0)
                        {
                            looser = true;
                            break;
                        }
                    }
                    if (looser)
                    {
                        printf("Game over! You lost!\n");
                    }
                    else
                    {
                        printf("Game over! You won!\n");
                    }
                    break;
                }

                current_player = 2;
            }

            // Server's turn
            if (current_player == 2)
            {
                valid_move = 0;
                while (!valid_move)
                {
                    ask_coordinates(&col, &row);
                    valid_move = make_move(col, row, destroyed, &destroyed_count, false);
                    if (!valid_move)
                    {
                        printf("Invalid move. Please try again.\n");
                    }
                }

                sprintf(buffer, "%c%d", col, row);
                send(new_socket, buffer, strlen(buffer), 0);
                display_board();

                printf("Destroyed squares: ");
                for (int i = 0; i < destroyed_count; i++)
                {
                    printf("%s ", destroyed[i]);
                }
                printf("\n");

                if (is_game_over())
                {
                    // We verify if A1 is stored in the destroyed array
                    bool looser = false;
                    for (int i = 0; i < destroyed_count; i++)
                    {
                        if (strcmp(destroyed[i], "A1") == 0)
                        {
                            looser = true;
                            break;
                        }
                    }
                    if (looser)
                    {
                        printf("Game over! You lost!\n");
                    }
                    else
                    {
                        printf("Game over! You won!\n");
                    }
                    break;
                }

                current_player = 1;
            }
        }

        close(new_socket);
        new_socket = -1;
        client_connected = 0;
        printf("Player disconnected. Exiting the game\n");
        exit(0);
    }

    close_sockets();
}

/**
 * @brief Function to launch the server with the IA as a player
 *
 * @param port The port number
 * @return void
 */
void play_network_server_ia(unsigned short port)
{
    struct sockaddr_in address, client_address;
    char buffer[1024] = {0};
    int addrlen = sizeof(address);
    int client_addrlen = sizeof(client_address);
    int client_connected = 0;

    // Buffer to store the destroyed squares
    char destroyed[ROWS * COLS][3];
    int destroyed_count = 0;

    printf("Creating socket...\n");
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Configuration of the socket to reuse the address
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt");
        close_sockets();
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    // Bind the socket to the port and address
    printf("Binding socket...\n");
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("Bind failed");
        close_sockets();
        exit(EXIT_FAILURE);
    }

    // Listen for 1 connection
    printf("Listening for connections...\n");
    if (listen(server_fd, 1) < 0)
    {
        perror("Listen failed");
        close_sockets();
        exit(EXIT_FAILURE);
    }

    printf("Waiting for a player to connect...\n");

    // Send a signal to the GUI to indicate that the server is ready
    pthread_mutex_lock(&server_mutex);
    server_ready = 1;
    pthread_cond_signal(&server_cond);
    pthread_mutex_unlock(&server_mutex);

    while (1)
    {
        if (client_connected)
        {
            printf("Server full: No more connections allowed.\n");
            sleep(1);
            continue;
        }

        printf("Accepting connection...\n");
        new_socket = accept(server_fd, (struct sockaddr *)&client_address, (socklen_t *)&client_addrlen);

        if (new_socket < 0)
        {
            perror("Accept failed");
            continue;
        }

        client_connected = 1;

        char *client_ip = inet_ntoa(client_address.sin_addr);
        printf("Player connected! IP: %s\n", client_ip);
        printf("\n");

        // Lauch the game GUI from the GTK thread
        printf("Launching game GUI...\n");
        g_idle_add((GSourceFunc)launch_game_gui_on_client_connect, NULL);

        printf("Initializing game board...\n");
        init_board();
        display_board();

        while (client_connected)
        {
            char col;
            int row;
            int valid_move = 0;
            int bytes_read;
            char *server_destroyed[ROWS * COLS];
            int server_destroyed_count = 0;

            // Client's turn
            if (current_player == 1)
            {
                // Allocate memory and copy the board to server_destroyed to get the destroyed before the client plays
                for (int i = 0; i < ROWS * COLS; i++)
                {
                    server_destroyed[i] = malloc(3 * sizeof(char));
                    if (server_destroyed[i] == NULL)
                    {
                        // Handle memory allocation failure
                        fprintf(stderr, "Memory allocation failed for server_destroyed[%d]\n", i);
                        exit(EXIT_FAILURE);
                    }
                    strcpy(server_destroyed[i], destroyed[i]);
                }
                server_destroyed_count = destroyed_count;
                printf("Waiting for the opponent's move...\n");

                valid_move = 0;
                while (!valid_move)
                {
                    memset(buffer, 0, sizeof(buffer));
                    bytes_read = read(new_socket, buffer, sizeof(buffer));

                    if (bytes_read == 0 || bytes_read == -1)
                    {
                        printf("Client disconnected.\n");
                        close(new_socket);
                        new_socket = -1;
                        client_connected = 0;
                        break;
                    }

                    sscanf(buffer, "%c%d", &col, &row);
                    printf("Opponent chose: %c%d\n", col, row);
                    valid_move = make_move(col, row, destroyed, &destroyed_count, true);
                    if (!valid_move)
                    {
                        printf("Invalid move received from opponent. Waiting for a new move...\n");
                    }
                }

                if (!client_connected)
                    break;

                display_board();
                printf("Destroyed squares: ");
                for (int i = 0; i < destroyed_count; i++)
                {
                    printf("%s ", destroyed[i]);
                }
                printf("\n");

                // Update the GUI with the opponent's move
                char *move_to_send = strdup(buffer);
                g_idle_add(update_gui_from_server, (gpointer)move_to_send);

                if (is_game_over())
                {
                    // We verify if A1 is stored in the destroyed array
                    bool looser = false;
                    for (int i = 0; i < server_destroyed_count; i++)
                    {
                        if (strcmp(server_destroyed[i], "A1") == 0)
                        {
                            looser = true;
                            break;
                        }
                    }
                    if (looser)
                    {
                        printf("Game over! You lost!\n");
                    }
                    else
                    {
                        printf("Game over! You won!\n");
                    }
                    break;
                }

                current_player = 2;
            }

            // Server's turn
            if (current_player == 2)
            {
                valid_move = 0;
                while (!valid_move)
                {
                    printf("The IA is playing...\n");
                    // We sleep for 1 second
                    sleep(1);
                    char *coordinates = ia_get_column(board);

                    col = coordinates[0];
                    row = coordinates[1] - '0';

                    // We verify that the coordinates are valid
                    if (row < 1 || row > 7 || col < 'A' || col > 'I')
                    {
                        bool valid = false;
                        // We loop until we get valid coordinates
                        while (!valid)
                        {
                            coordinates = ia_get_column(board);
                            col = coordinates[0];
                            row = coordinates[1] - '0';
                            if (row >= 1 && row <= 7 && col >= 'A' && col <= 'I')
                            {
                                valid = true;
                            }
                        }
                    }

                    valid_move = make_move(col, row, destroyed, &destroyed_count, true);

                    // We loop until we get a valid move

                    if (!valid_move)
                    {
                        while (!valid_move)
                        {
                            coordinates = ia_get_column(board);
                            col = coordinates[0];
                            row = coordinates[1] - '0';
                            valid_move = make_move(col, row, destroyed, &destroyed_count, true);
                        }
                    }
                }

                char *ia_coords = malloc(50 * sizeof(char));
                sprintf(ia_coords, "The IA choosed : %c%d", col, row);
                printf("%s\n", ia_coords);
                free(ia_coords);
                printf("The IA made a successful move !\n");

                sprintf(buffer, "%c%d", col, row);
                send(new_socket, buffer, strlen(buffer), 0);
                display_board();

                printf("Destroyed squares: ");
                for (int i = 0; i < destroyed_count; i++)
                {
                    printf("%s ", destroyed[i]);
                }
                printf("\n");

                if (is_game_over())
                {
                    // We verify if A1 is stored in the destroyed array
                    bool looser = false;
                    for (int i = 0; i < destroyed_count; i++)
                    {
                        if (strcmp(destroyed[i], "A1") == 0)
                        {
                            looser = true;
                            break;
                        }
                    }
                    if (looser)
                    {
                        printf("Game over! You lost!\n");
                    }
                    else
                    {
                        printf("Game over! You won!\n");
                    }
                    break;
                }

                current_player = 1;
            }
        }

        close(new_socket);
        new_socket = -1;
        client_connected = 0;
        printf("Player disconnected. Exiting the game\n");
        exit(0);
    }

    close_sockets();
}

/**
 * @brief Function to launch the server thread
 *
 * @param arg The port number as a void pointer
 * @return void*
 */
void *launch_server_thread(void *arg)
{
    printf("Starting server thread...\n");
    unsigned short server_port = *(unsigned short *)arg;
    printf("Server port: %d\n", server_port);
    play_network_server(server_port);
    // Free the memory allocated for the port number
    free(arg);
    return NULL;
}

/**
 * @brief Function to start the server from the GUI
 *
 * @param port The port number
 * @return void
 */
void start_server_from_gui(unsigned short port)
{
    // Get the CTRL+C signal to close properly the sockets
    signal(SIGINT, handle_sigint);

    pthread_t server_thread;
    unsigned short *server_port_ptr = malloc(sizeof(unsigned short));
    if (server_port_ptr == NULL)
    {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    *server_port_ptr = port;
    if (pthread_create(&server_thread, NULL, launch_server_thread, (void *)server_port_ptr) != 0)
    {
        perror("Failed to create server thread");
        free(server_port_ptr);
        exit(EXIT_FAILURE);
    }

    // Wait for the server to be ready
    pthread_mutex_lock(&server_mutex);
    while (!server_ready)
    {
        pthread_cond_wait(&server_cond, &server_mutex);
    }
    pthread_mutex_unlock(&server_mutex);

    printf("Server is ready, GUI can continue...\n");
    // Detach the server thread to be independent
    pthread_detach(server_thread);
}
