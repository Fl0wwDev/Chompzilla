#define IA_H
#ifdef IA_H
#include "../inc/const.h"

int squares_to_remove(const int *coo, int tab[ROWS][COLS], int rows, int cols);
int *randomCoordinates(int rows, int cols, int tab[ROWS][COLS]);
char *ia_get_column(int tab[ROWS][COLS]);

#endif