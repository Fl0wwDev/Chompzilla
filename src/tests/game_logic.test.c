#include <stdio.h>
#include <criterion/criterion.h>
#include "../inc/const.h"
#include "../game_logic/game_logic.h"

/**
 * @brief Test if the board array exists
 */
Test(game, test_board_exists)
{
    extern int board[ROWS][COLS];
    cr_assert_not_null(board, "board array does not exist");
}

/**
 * @brief Test if the init_board function exists
 */
Test(game, test_init_board_exists)
{
    cr_assert_not_null(init_board, "init_board function does not exist");
}

/**
 * @brief Test if the init_board function sets all squares to 1
 */
Test(game, test_init_board_contains_only_ones)
{
    // Get the board array from game.c
    extern int board[ROWS][COLS];
    // Call the init_board function from game.c
    init_board();
    // Check if all squares are set to 1
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            cr_assert_eq(board[i][j], 1, "Square at (%d, %d) is not set to 1", i, j);
        }
    }
}

/**
 * @brief Test if the board respects the ROWS and COLS constants
 */
Test(game, test_board_dimensions)
{
    // Get the board array from game.c
    extern int board[ROWS][COLS];
    // Check if the board array has the correct dimensions
    cr_assert_eq(sizeof(board) / sizeof(board[0]), ROWS, "The board has an incorrect number of rows");
    cr_assert_eq(sizeof(board[0]) / sizeof(board[0][0]), COLS, "The board has an incorrect number of columns");
}

/**
 * @brief Test if the display_board function exists
 */
Test(game, test_display_board_exists)
{
    cr_assert_not_null(display_board, "display_board function does not exist");
}

/**
 * @brief Test if the display_board function displays the board correctly (row and column numbers)
 */
Test(game, test_display_board_row_col)
{
    // Redirect stdout to a file
    freopen("/tmp/test_display_board_row_col", "w+", stdout);
    // Call the display_board function from game.c
    display_board();
    // Reopen the stdout to the terminal
    freopen("/dev/tty", "w", stdout);
    // Open the file in read mode
    FILE* f = fopen("/tmp/test_display_board_row_col", "r");
    // Check if the file was opened
    cr_assert_not_null(f, "Could not open file /tmp/test_display_board_row_col");
    // Check if the board was displayed correctly
    char line[100];
    int i = 0;
    while (fgets(line, sizeof(line), f))
    {
        if (i == 0)
        {
            cr_assert_str_eq(line, "  A B C D E F G H I\n", "The board is not displayed correctly");
        }
        else
        {
            cr_assert_eq(line[0], i + '0', "The board is not displayed correctly");
        }
        i++;
    }
    // Close the file
    fclose(f);
}

/**
 * @brief Test if the board contains only 1s after calling the init_board function
 */
Test(game, test_display_board_only_ones)
{
    // Redirect stdout to a file
    freopen("/tmp/test_display_board_only_ones", "w+", stdout);
    // Call the init_board function from game.c
    init_board();
    // Call the display_board function from game.c
    display_board();
    // Reopen the stdout to the terminal
    freopen("/dev/tty", "w", stdout);
    // Open the file in read mode
    FILE* f = fopen("/tmp/test_display_board_only_ones", "r");
    // Check if the file was opened
    cr_assert_not_null(f, "Could not open file /tmp/test_display_board_only_ones");
    // Check if the board was displayed correctly
    char line[100];
    int i = 0;
    while (fgets(line, sizeof(line), f))
    {
        if (i > 0)
        {
            for (int j = 0; j < COLS; j++)
            {
                cr_assert_eq(line[j * 2 + 2], '1', "The board is not displayed correctly");
            }
        }
        i++;
    }
    // Close the file
    fclose(f);
}

/**
 * @brief Test if the column_to_index function exists
 */
Test(game, test_column_to_index_exists)
{
    cr_assert_not_null(column_to_index, "column_to_index function does not exist");
}

/**
 * @brief Test if the column_to_index function returns the correct index for a given column
 */
Test(game, test_column_to_index)
{
    cr_assert_eq(column_to_index('A'), 0, "Column A should return 0");
    cr_assert_eq(column_to_index('B'), 1, "Column B should return 1");
    cr_assert_eq(column_to_index('C'), 2, "Column C should return 2");
    cr_assert_eq(column_to_index('D'), 3, "Column D should return 3");
    cr_assert_eq(column_to_index('E'), 4, "Column E should return 4");
    cr_assert_eq(column_to_index('F'), 5, "Column F should return 5");
    cr_assert_eq(column_to_index('G'), 6, "Column G should return 6");
    cr_assert_eq(column_to_index('H'), 7, "Column H should return 7");
    cr_assert_eq(column_to_index('I'), 8, "Column I should return 8");
}

/**
 * @brief Test if the count_squares_to_remove function exists
 */
Test(game, test_count_squares_to_remove_exists)
{
    cr_assert_not_null(count_squares_to_remove, "count_squares_to_remove function does not exist");
}

/**
 * @brief Test if the count_squares_to_remove function returns the correct number of squares to remove
 */
Test(game, test_count_squares_to_remove)
{
    // Get the board array from game.c
    extern int board[ROWS][COLS];
    // Call the init_board function from game.c
    init_board();
    // Check if we delete I7, we should delete 1 square
    cr_assert_eq(count_squares_to_remove(0, 8), 1, "The number of squares to remove is incorrect for I7");
    // Check if we delete H6, we should delete 4 squares
    cr_assert_eq(count_squares_to_remove(1, 7), 4, "The number of squares to remove is incorrect for H6");
    // Check if we delete A1, we should delete 63 squares
    cr_assert_eq(count_squares_to_remove(6, 0), 63, "The number of squares to remove is incorrect for A1");
}

/**
 * @brief Test if the destroy_squares function exists
 */
Test(game, test_destroy_squares_exists)
{
    cr_assert_not_null(destroy_squares, "destroy_squares function does not exist");
}

/**
 * @brief Test if the destroy_squares function removes the correct number of squares for I7
 */
Test(game, test_destroy_squares_I7)
{
    // Get the board array from game.c
    extern int board[ROWS][COLS];
    // Call the init_board function from game.c
    init_board();
    // Create a destroyed array
    char destroyed[ROWS * COLS][3];
    int destroyed_count = 0;
    // Check if we delete I7, we should delete 1 square
    cr_assert_eq(destroy_squares(0, 8, destroyed, &destroyed_count), 1, "The number of squares removed is incorrect for I7");
}

/**
 * @brief Test if the destroy_squares function removes the correct number of squares for H6
 */
Test(game, test_destroy_squares_H6)
{
    // Get the board array from game.c
    extern int board[ROWS][COLS];
    // Call the init_board function from game.c
    init_board();
    // Create a destroyed array
    char destroyed[ROWS * COLS][3];
    int destroyed_count = 0;
    // Check if we delete H6, we should delete 4 squares
    cr_assert_eq(destroy_squares(1, 7, destroyed, &destroyed_count), 4, "The number of squares removed is incorrect for H6");
}

/**
 * @brief Test if the destroy_squares function removes the correct number of squares for A1
 */
Test(game, test_destroy_squares_A1)
{
    // Get the board array from game.c
    extern int board[ROWS][COLS];
    // Call the init_board function from game.c
    init_board();
    // Create a destroyed array
    char destroyed[ROWS * COLS][3];
    int destroyed_count = 0;
    // Check if we delete A1, we should delete 63 squares
    cr_assert_eq(destroy_squares(6, 0, destroyed, &destroyed_count), 63, "The number of squares removed is incorrect for A1");
}

/**
 * @brief Test if the make_move function exists
 */
Test(game, test_make_move_exists)
{
    cr_assert_not_null(make_move, "make_move function does not exist");
}

/**
 * @brief Test if the make_move function returns 0 for invalid coordinates
 */
Test(game, test_make_move_invalid_coordinates)
{
    // Get the board array from game.c
    extern int board[ROWS][COLS];
    // Call the init_board function from game.c
    init_board();
    // Create a destroyed array
    char destroyed[ROWS * COLS][3];
    int destroyed_count = 0;
    // Check if we make a move with invalid coordinates
    cr_assert_eq(make_move('A', 8, destroyed, &destroyed_count, false), 0, "The move should return 0 for invalid coordinates");
}

/**
 * @brief Test if the make_move function returns 0 for already destroyed squares
 */
Test(game, test_make_move_already_destroyed_square)
{
    // Get the board array from game.c
    extern int board[ROWS][COLS];
    // Call the init_board function from game.c
    init_board();
    // Create a destroyed array
    char destroyed[ROWS * COLS][3];
    int destroyed_count = 0;
    // We destroy I7
    destroy_squares(0, 8, destroyed, &destroyed_count);
    // Check if we make a move with already destroyed squares
    cr_assert_eq(make_move('I', 7, destroyed, &destroyed_count, false), 0, "The move should return 0 for already destroyed squares");
}

/**
 * @brief Test if the make_move function returns 1 for a valid move
 */
Test(game, test_make_move_valid_move)
{
    // Get the board array from game.c
    extern int board[ROWS][COLS];
    // Call the init_board function from game.c
    init_board();
    // Create a destroyed array
    char destroyed[ROWS * COLS][3];
    int destroyed_count = 0;
    // Check if we make a valid move
    cr_assert_eq(make_move('I', 7, destroyed, &destroyed_count, false), 1, "The move should return 1 for a valid move");
}

/**
 * @brief Test if the is_game_over function exists
 */
Test(game, test_is_game_over_exists)
{
    cr_assert_not_null(is_game_over, "is_game_over function does not exist");
}

/**
 * @brief Test if the is_game_over function returns 0 when the game is not over
 */
Test(game, test_is_game_over_not_over)
{
    // Get the board array from game.c
    extern int board[ROWS][COLS];
    // Call the init_board function from game.c
    init_board();
    // Check if the game is not over
    cr_assert_eq(is_game_over(), 0, "The game should not be over");
}

/**
 * @brief Test if the is_game_over function returns 1 when the game is over
 */
Test(game, test_is_game_over_over)
{
    // Get the board array from game.c
    extern int board[ROWS][COLS];
    // Call the init_board function from game.c
    init_board();
    // Create a destroyed array
    char destroyed[ROWS * COLS][3];
    int destroyed_count = 0;
    // Destroy all squares
    for (int i = 0; i < ROWS; i++)
    {
        for (int j = 0; j < COLS; j++)
        {
            destroy_squares(i, j, destroyed, &destroyed_count);
        }
    }
    // Check if the game is over
    cr_assert_eq(is_game_over(), 1, "The game should be over");
}

/**
 * @brief Test if the ask_coordinates function exists
 */
Test(game, test_ask_coordinates_exists)
{
    cr_assert_not_null(ask_coordinates, "ask_coordinates function does not exist");
}

/**
 * @brief Test if the ask_coordinates gets the correct coordinates
 * We open a file with the expected coordinates and redirect stdin to it
 * We call the ask_coordinates function and check if the coordinates are correct in the two pointers passed as arguments
 * We redirect stdin back to the terminal
 */
Test(game, test_ask_coordinates)
{
    // Open the file with the expected coordinates
    FILE* f = fopen("/tmp/test_ask_coordinates", "w+");
    fprintf(f, "I7\n");
    fflush(f);
    rewind(f);

    // Redirect stdin to the file
    freopen("/tmp/test_ask_coordinates", "r", stdin);

    // Create the variables to store the coordinates
    char col;
    int row;

    // Call the ask_coordinates function
    ask_coordinates(&col, &row);

    // Check if the coordinates are correct
    cr_assert_eq(col, 'I', "The column is incorrect");
    cr_assert_eq(row, 7, "The row is incorrect");

    // Reopen stdin to the terminal
    freopen("/dev/tty", "r", stdin);

    // Close the file
    fclose(f);
}

/**
 * @brief Test if the play_local_game function exists
 */
Test(game, test_play_local_game_exists)
{
    cr_assert_not_null(play_local_game, "play_local_game function does not exist");
}

/**
 * @brief Test if the play_local_game_gui function exists
 */
Test(game, test_play_local_game_gui_exists)
{
    cr_assert_not_null(play_local_game_gui, "play_local_game_gui function does not exist");
}

/**
 * @brief Test if the local_game_gui_logic function exists
 */
Test(game, test_local_game_gui_logic_exists)
{
    cr_assert_not_null(local_game_gui_logic, "local_game_gui_logic function does not exist");
}

/**
 * @brief Test if the local_game_gui_logic function returns a ServerResponse struct
 */
Test(game, test_local_game_gui_logic_returns_struct)
{
    // Call the local_game_gui_logic function
    ServerResponse response = local_game_gui_logic("I7", 1);
    // Check if the function returns a ServerResponse struct
    cr_assert_not_null(&response, "The function should return a ServerResponse struct");
}

/**
 * @brief Test if the local_game_gui_logic function returns a ServerResponse struct in case of invalid coordinates
 */
Test(game, test_local_game_gui_logic_invalid_coordinates)
{
    // Call the local_game_gui_logic function with invalid coordinates
    ServerResponse response = local_game_gui_logic("I8", 1);
    // Check if the function returns a ServerResponse struct
    cr_assert_eq(response.coordinates_size, 0, "The function should return a ServerResponse struct with coordinates_size = 0");
    cr_assert_eq(response.coordinates, NULL, "The function should return a ServerResponse struct with coordinates = NULL");
    // Check that error contains "Invalid coordinates"
    cr_assert_str_eq(response.error, "Invalid coordinates", "The function should return a ServerResponse struct with error = 'Invalid coordinates'");
    cr_assert_eq(response.player_num, 1, "The function should return a ServerResponse struct with player_num = 1");
    cr_assert_eq(response.is_game_over, false, "The function should return a ServerResponse struct with is_game_over = false");
}

/**
 * @brief Test if the local_game_gui_logic function returns a ServerResponse struct in case of invalid move
 */
Test(game, test_local_game_gui_logic_invalid_move)
{
    // Call the local_game_gui_logic function with invalid move
    ServerResponse response = local_game_gui_logic("A1", 1);
    // Check if the function returns a ServerResponse struct
    cr_assert_eq(response.coordinates_size, 0, "The function should return a ServerResponse struct with coordinates_size = 0");
    cr_assert_eq(response.coordinates, NULL, "The function should return a ServerResponse struct with coordinates = NULL");
    // Check that error contains "Invalid move, you can only remove up to 5 squares"
    cr_assert_str_eq(response.error, "Invalid move, you can only remove up to 5 squares", "The function should return a ServerResponse struct with error = 'Invalid move, you can only remove up to 5 squares'");
    cr_assert_eq(response.player_num, 1, "The function should return a ServerResponse struct with player_num = 1");
    cr_assert_eq(response.is_game_over, false, "The function should return a ServerResponse struct with is_game_over = false");
}
