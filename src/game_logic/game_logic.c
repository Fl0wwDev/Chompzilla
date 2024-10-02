#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include "game_logic.h"
#include "../inc/const.h"
#include "../gui/gui.h"
#include "../ia/ia.h"

int board[ROWS][COLS];

/**
 * @brief Function to initialize the game board, it use the constant ROWS and COLS defined in inc/const.h
 */
void init_board()
{
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            board[i][j] = 1;
        }
    }
}

/**
 * @brief Function to display the game board in the CLI
 */
void display_board()
{
    printf("  A B C D E F G H I\n");
    for (int i = 0; i < ROWS; i++)
    {
        printf("%d ", i + 1);
        for (int j = 0; j < COLS; j++)
        {
            printf("%d ", board[ROWS - 1 - i][j]);
        }
        printf("\n");
    }
}

/**
 * @brief Function to convert a column letter to an index
 *
 * @param col The column letter to convert
 * @return int The index of the column
 */
int column_to_index(char col)
{
    return toupper(col) - 'A';
}

/**
 * @brief Function to count the number of squares to remove
 *
 * @param row The row of the square to remove
 * @param col The column of the square to remove
 * @return int The number of squares to remove
 */
int count_squares_to_remove(int row, int col)
{
    int squares_count = 0;
    for (int i = row; i >= 0; i--)
    {
        for (int j = col; j < COLS; j++)
        {
            if (board[i][j] == 1)
            {
                squares_count++;
            }
        }
    }
    return squares_count;
}

/**
 * @brief Function to destroy the squares
 *
 * @param row The row of the square to remove
 * @param col The column of the square to remove
 * @param destroyed The array to store the destroyed squares
 */
int destroy_squares(int row, int col, char destroyed[][3], int *destroyed_count)
{
    int squares_removed = 0;
    *destroyed_count = 0;

    for (int i = row; i >= 0; i--)
    {
        for (int j = col; j < COLS; j++)
        {
            if (board[i][j] == 1)
            {
                board[i][j] = 0;
                squares_removed++;

                destroyed[*destroyed_count][0] = 'A' + j;
                destroyed[*destroyed_count][1] = '0' + (ROWS - i);
                destroyed[*destroyed_count][2] = '\0';

                (*destroyed_count)++;
            }
        }
    }
    return squares_removed;
}

/**
 * @brief Function to make a move
 *
 * @param col_letter The column letter of the move
 * @param row The row of the move
 * @param destroyed The array to store the destroyed squares
 * @param destroyed_count The number of destroyed squares
 * @return int The destroyed squares
 */
int make_move(char col_letter, int row, char destroyed[][3], int *destroyed_count, bool is_ia)
{
    if (!is_ia)
    {
        printf("Making move %c%d\n", col_letter, row);
    }

    // Debug : Vérifier la colonne et la ligne
    if (row == -1)
    {
        if (!is_ia)
        {
            printf("Error: Received invalid row -1\n");
        }
        return 0;
    }

    int col = column_to_index(col_letter);
    row = ROWS - row;

    if (row < 0 || row >= ROWS || col < 0 || col >= COLS)
    {
        if (!is_ia)
        {
            printf("Invalid coordinates\n");
        }
        return 0;
    }
    if (board[row][col] == 0)
    {
        if (!is_ia)
        {
            printf("The square is already destroyed. Choose another square.\n");
        }
        return 0;
    }

    int squares_to_remove = count_squares_to_remove(row, col);
    if (squares_to_remove > MAX_SQUARES_REMOVED)
    {
        if (!is_ia)
        {
            printf("Invalid move. Too many squares removed (max 5)\n");
        }
        return 0;
    }

    return destroy_squares(row, col, destroyed, destroyed_count);
}

/**
 * @brief Function to check if the game is over
 *
 * @return int 1 if the game is over, 0 otherwise
 */
int is_game_over()
{
    return board[ROWS - 1][0] == 0;
}

/**
 * @brief Function to ask the user for coordinates
 *
 * @param col The pointer to the column letter
 * @param row The pointer to the row number
 */
void ask_coordinates(char *col, int *row)
{
    printf("Enter the coordinates : ");
    char input[4];

    // Si l'entrée est vide, ne pas continuer
    if (fgets(input, sizeof(input), stdin) == NULL || input[0] == '\n' || input[0] == '\0')
    {
        printf("Error: Empty input received\n");
        *row = -1;
        return;
    }

    // Processus normal de parsing
    *col = toupper(input[0]);

    // Debug: Affichage de l'input pour mieux comprendre le problème
    printf("Input coordinates: %s\n", input);

    if (sscanf(input + 1, "%d", row) != 1 || *col < 'A' || *col > 'I' || *row < 1 || *row > 7)
    {
        printf("Parsed coordinates: Invalid\n");
        *row = -1;
    }
    else
    {
        // Debug: Affichage après parsing
        printf("Parsed coordinates: %c%d\n", *col, *row);
    }
}

/**
 * @brief Function to play a local game in CLI
 *
 * @return void
 */
void play_local_game()
{
    init_board();
    display_board();

    int current_player = 1;
    char destroyed[ROWS * COLS][3];
    int destroyed_count = 0;

    while (1)
    {
        char col;
        int row;
        int valid_move = 0;

        while (!valid_move)
        {
            printf("Player %d, enter your coordinates:\n", current_player);
            ask_coordinates(&col, &row);
            if (row == -1)
            {
                printf("Invalid coordinates. Please try again.\n");
                continue;
            }
            valid_move = make_move(col, row, destroyed, &destroyed_count, false);
            if (!valid_move)
            {
                printf("Invalid move. Please try again.\n");
            }
        }

        printf("Player %d made a successful move!\n", current_player);
        display_board();

        printf("Destroyed squares: ");
        for (int i = 0; i < destroyed_count; i++)
        {
            printf("%s ", destroyed[i]);
        }
        printf("\n");

        if (is_game_over())
        {
            printf("Game over! Player %d loses!\n", current_player);
            printf("Player %d wins!\n", current_player == 1 ? 2 : 1);
            break;
        }

        current_player = (current_player == 1) ? 2 : 1;
    }
}

/**
 * @brief Function to play a local game in CLI against the IA
 *
 * @return void
 */
void play_local_game_ia()
{
    init_board();
    display_board();

    int current_player = 1;
    char destroyed[ROWS * COLS][3];
    int destroyed_count = 0;

    while (1)
    {
        if (current_player == 1)
        {
            char col;
            int row;
            int valid_move = 0;

            while (!valid_move)
            {
                printf("Player %d, enter your coordinates:\n", current_player);
                ask_coordinates(&col, &row);
                if (row == -1)
                {
                    printf("Invalid coordinates. Please try again.\n");
                    continue;
                }
                valid_move = make_move(col, row, destroyed, &destroyed_count, true);
                if (!valid_move)
                {
                    printf("Invalid move. Please try again.\n");
                }
            }

            printf("Player %d made a successful move!\n", current_player);
            display_board();

            printf("Destroyed squares: ");
            for (int i = 0; i < destroyed_count; i++)
            {
                printf("%s ", destroyed[i]);
            }
            printf("\n");

            if (is_game_over())
            {
                printf("Game over! Player %d loses!\n", current_player);
                printf("Player %d wins!\n", current_player == 1 ? 2 : 1);
                break;
            }

            current_player = (current_player == 1) ? 2 : 1;
        }
        else
        {
            char col;
            int row;
            int valid_move = 0;

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

                printf("The IA made a successful move!\n");
                display_board();

                printf("Destroyed squares: ");
                for (int i = 0; i < destroyed_count; i++)
                {
                    printf("%s ", destroyed[i]);
                }
                printf("\n");

                if (is_game_over())
                {
                    printf("Game over! Player %d loses!\n", current_player);
                    printf("Player %d wins!\n", current_player == 1 ? 2 : 1);
                    break;
                }

                current_player = (current_player == 1) ? 2 : 1;
            }
        }
    }
}

/**
 * @brief Function to play a local game with GUI
 *
 * @return void
 */
void play_local_game_gui()
{
    init_board();
    launch_gui(0, NULL);
}

/**
 * @brief Function to handle the game logic for the GUI
 *
 * @param coordinates The coordinates of the move
 * @param current_player The current player
 * @return ServerResponse The response struct to send to the GUI
 */
ServerResponse local_game_gui_logic(const char *coordinates, int current_player)
{
    char destroyed[ROWS * COLS][3];
    int destroyed_count = 0;
    int valid_move = 0;

    // Vérification des coordonnées
    if (coordinates == NULL || coordinates[0] < 'A' || coordinates[0] > 'I' || coordinates[1] < '1' || coordinates[1] > '7')
    {
        ServerResponse response;
        response.coordinates_size = 0;
        response.coordinates = NULL;
        response.error = "Invalid coordinates";
        response.player_num = current_player;
        response.is_game_over = false;
        return response;
    }

    char col = coordinates[0];
    int row = coordinates[1] - '0';

    valid_move = make_move(col, row, destroyed, &destroyed_count, false);

    if (!valid_move)
    {
        ServerResponse response;
        response.coordinates_size = 0;
        response.coordinates = NULL;
        response.error = "Invalid move, you can only remove up to 5 squares";
        response.player_num = current_player;
        response.is_game_over = false;
        return response;
    }

    if (is_game_over())
    {
        ServerResponse response;
        response.coordinates_size = 0;
        response.coordinates = NULL;
        char current_player_char[2];
        sprintf(current_player_char, "%d", current_player);
        response.error = g_strconcat("Game over! Player ", current_player_char, " loses!", NULL);
        response.player_num = current_player;
        response.is_game_over = true;
        return response;
    }

    ServerResponse response;
    response.coordinates_size = destroyed_count;
    response.coordinates = malloc(destroyed_count * sizeof(char *));
    for (int i = 0; i < response.coordinates_size; i++)
    {
        response.coordinates[i] = malloc(3 * sizeof(char));
        strcpy(response.coordinates[i], destroyed[i]);
    }
    response.error = NULL;

    current_player = (current_player == 1) ? 2 : 1;
    response.player_num = current_player;
    response.is_game_over = false;

    return response;
}

/**
 * @brief Function to update the GUI after a move
 *
 * @param coordinates The coordinates of the move
 * @return int 1 if the GUI was updated, 0 otherwise
 */
void update_gui_after_move(const char *coordinates)
{
    // Alloue de la mémoire pour envoyer les coordonnées à la fonction GUI
    char *coordinates_copy = g_strdup(coordinates);

    // Utilise g_idle_add pour mettre à jour l'UI depuis un autre thread
    g_idle_add(update_gui_from_server, coordinates_copy);
}
