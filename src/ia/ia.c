#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "../inc/const.h"
#include "ia.h"

int coo[3];
char *result;


/**
 * @brief Function to get the number of squares removed
 *
 * @param coo The coordinates
 * @param tab The game board
 * @param rows The number of rows
 * @param cols The number of columns
 * @return int The number of squares removed
 */
int squares_to_remove(const int *coo, int tab[ROWS][COLS], int rows, int cols)
{
    int count = 0;
    for (int i = coo[0]; i < rows; i++)
    {
        for (int j = coo[1]; j < cols; j++)
        {
            count += tab[i][j];
            if (count == 6)
            {
                return count;
            }
        }
    }
    return count;
}

/**
 * @brief Function to get a random coordinate
 *
 * @param rows The number of rows
 * @param cols The number of columns
 * @param tab The game board
 * @return int* The random coordinate
 */
int *randomCoordinates(int rows, int cols, int tab[ROWS][COLS])
{

    // Array to store valid coordinates in byte - MAX_LIST_SIZE arbitrary
    int liste[MAX_LIST_SIZE * 8][3];
    int count = 0;
    coo[0] = rows;

    for (int i = 0; i < rows; i++)
    {
        coo[0] = coo[0] - 1;
        coo[1] = cols;
        for (int j = 0; j < cols; j++)
        {
            coo[1] = coo[1] - 1;
            int var_nbr_casse = squares_to_remove(coo, tab, rows, cols);
            if ( var_nbr_casse > 0)
            {
                if (var_nbr_casse <= 5)
                {
                    liste[count][0] = coo[0];
                    liste[count][1] = coo[1];
                    count++;
                    if (count == MAX_LIST_SIZE)
                    {
                        break;
                    }
                }
                else
                    break;
            }
        }
        if (count == MAX_LIST_SIZE)
            break;
    }
    // Choose a random index
    int randomIndex = rand() % count;
    coo[0] = liste[randomIndex][0];
    coo[1] = liste[randomIndex][1];

    return coo;
}

/**
 * @brief Main function to get a column
 *
 * @param tab The game board
 * @return char The coordinates like A1
 */
char *ia_get_column(int tab[ROWS][COLS])
{
    result = malloc(4 * sizeof(char));
    // Intialize random number generator with the current time
    srand(time(NULL));

    int *coord = randomCoordinates(ROWS, COLS, tab);

    const char lettre[] = "ABCDEFGHI";
    int row = coord[0] + 1;
    char col = lettre[coord[1]];

    sprintf(result, "%c%d", col, row);
    return result;
}