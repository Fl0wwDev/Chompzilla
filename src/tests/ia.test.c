#include <stdio.h>
#include <criterion/criterion.h>
#include "../inc/const.h"
#include "../ia/ia.h"

extern int coo[3];

/**
 * @brief Test the existence of the squares_to_remove function
 */
Test(ia, correct_counter)
{
        cr_assert_not_null(coo, "coordinate array does not exist");
}

/**
 * @brief Test if the function returns the correct number of squares to remove
 */
Test(ia, correct_number_of_squares_removed)
{
        int tab[ROWS][COLS] = {1};
        for (int i = 0; i < 7; i++)
        {
                for (int j = 0; j < 9; j++) // every possible board
                {
                        for (int x = 0; x < 7; x++)
                        {
                                for (int y = 0; y < 9; y++) // every possible coordinates
                                {
                                        tab[i][j] = 0; // whe replace each 1 by a 0 one by>
                                        coo[0] = x;
                                        coo[1] = y;
                                        cr_expect(squares_to_remove(coo, tab, ROWS, COLS) <= 5, "cant delete more than 5 squares");

                                        cr_expect(squares_to_remove(coo, tab, ROWS, COLS) >= 0, "must delete at least 1 square");
                                }
                        }
                }
        }
}
