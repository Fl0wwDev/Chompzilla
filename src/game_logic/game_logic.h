#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdbool.h>
#include "../inc/const.h"
#include "../gui/gui.h"

// Declare the game board as an external variable
extern int board[ROWS][COLS];

// Function prototypes
void init_board();
void display_board();
int column_to_index(char col);
int count_squares_to_remove(int row, int col);
int destroy_squares(int row, int col, char destroyed[][3], int *destroyed_count);
int make_move(char col_letter, int row, char destroyed[][3], int *destroyed_count, bool is_ia);
int is_game_over();
void ask_coordinates(char *col, int *row);
void play_local_game();
void play_local_game_ia();
void play_local_game_gui();
ServerResponse local_game_gui_logic(const char* coordinates, int current_player);
void update_gui_after_move(const char* coordinates);

#endif
