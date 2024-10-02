#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include "client.h"
#include "../game_logic/game_logic.h"
#include "../inc/const.h"
#include "../ia/ia.h"

/**
 * @brief Function to launch the client on a specific IP address and port
 *
 * @param server_ip The IP address of the server
 * @param port The port number
 * @return void
 */
void play_network_client(char *server_ip, unsigned short port)
{
    int client_fd;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};
    // Array to store destroyed squares
    char destroyed[10][3];
    // Counter for destroyed squares
    int destroyed_count = 0;

    // Create the socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    // Convert the server IP address
    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0)
    {
        printf("Invalid address / Address not supported\n");
        return;
    }

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Connection failed");
        return;
    }

    printf("Connected to the server at %s!\n", server_ip);

    init_board();
    display_board();

    while (1)
    {
        char col;
        int row;
        int valid_move = 0;

        // Client's turn (player 1) - The client starts first
        valid_move = 0;
        while (!valid_move)
        {
            ask_coordinates(&col, &row);
            // Validate if the move is correct
            valid_move = make_move(col, row, destroyed, &destroyed_count, false);
            if (!valid_move)
            {
                printf("Invalid move. Please try again.\n");
            }
        }
        printf("Move successful! Sending to server...\n");
        sprintf(buffer, "%c%d", col, row);
        // Send the move to the server
        send(client_fd, buffer, strlen(buffer), 0);
        display_board();

        // Check if the client won
        if (is_game_over())
        {
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
        }

        // Server's turn (player 2) - The client waits for the server's move
        printf("Waiting for the opponent's move...\n");
        memset(buffer, 0, sizeof(buffer));
        // Receive the server's move
        read(client_fd, buffer, sizeof(buffer));
        // Parse the move coordinates
        sscanf(buffer, "%c%d", &col, &row);
        printf("Opponent chose: %c%d\n", col, row);
        // Update the board with the server's move
        make_move(col, row, destroyed, &destroyed_count, false);
        display_board(); // Display the updated board

        // Check if the server won
        if (is_game_over())
        {
            printf("Congrats! you won!\n");
            break;
        }
    }

    // Close the client socket
    close(client_fd);
}

/**
 * @brief Function to launch the client in IA mode on a specific IP address and port
 *
 * @param server_ip The IP address of the server
 * @param port The port number
 * @return void
 */
void play_network_ia_client(char *server_ip, unsigned short port)
{
    int client_fd;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};
    // Array to store destroyed squares
    char destroyed[10][3];
    // Counter for destroyed squares
    int destroyed_count = 0;

    // Create the socket
    if ((client_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    // Convert the server IP address
    if (inet_pton(AF_INET, server_ip, &server_address.sin_addr) <= 0)
    {
        printf("Invalid address / Address not supported\n");
        return;
    }

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("Connection failed");
        return;
    }

    printf("Connected to the server at %s!\n", server_ip);

    init_board();
    display_board();

    while (1)
    {
        char col;
        int row;
        int valid_move = 0;

        // Client's turn (player 1) - The client starts first
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
        printf("The IA made a successful move! Sending to server...\n");
        sprintf(buffer, "%c%d", col, row);
        // Send the move to the server
        send(client_fd, buffer, strlen(buffer), 0);
        display_board();

        // Check if the client won
        if (is_game_over())
        {
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
        }

        // Server's turn (player 2) - The client waits for the server's move
        printf("Waiting for the opponent's move...\n");
        memset(buffer, 0, sizeof(buffer));
        // Receive the server's move
        read(client_fd, buffer, sizeof(buffer));
        // Parse the move coordinates
        sscanf(buffer, "%c%d", &col, &row);
        printf("Opponent chose: %c%d\n", col, row);
        // Update the board with the server's move
        make_move(col, row, destroyed, &destroyed_count, true);
        display_board(); // Display the updated board

        // Check if the server won
        if (is_game_over())
        {
            printf("Congrats! you won!\n");
            break;
        }
    }

    // Close the client socket
    close(client_fd);
}
