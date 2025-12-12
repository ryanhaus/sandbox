#include "minefield.h"
#include <time.h>
#include <stdlib.h>

minefield_t create_empty_minefield(int width, int height)
{
    // Allocate memory for mines
    bool** mines = calloc(width, sizeof(bool*));

    for (int i = 0; i < width; i++)
    {
        mines[i] = calloc(height, sizeof(bool));
    }

    // Return struct
    minefield_t minefield = {
        width,
        height,
        mines,
    };

    return minefield;
}

minefield_t create_filled_minefield(int width, int height, int n_mines)
{
    // Create empty minefield, then fill it with mines
    minefield_t minefield = create_empty_minefield(width, height);

    srand(time(NULL));

    while (n_mines != 0)
    {
        // Pick random row/col, if empty, fill it with a mine and decrease counter
        int col = rand() % width;
        int row = rand() % height;

        if (minefield.mines[col][row] == false)
        {
            minefield.mines[col][row] = true;
            n_mines--;
        }
    }

    return minefield;
}

char* get_minefield_str(minefield_t minefield)
{
    // each row will need 'width' characters + 1 for \n, and the entire string will need +1 for terminating char
    int n_chars = minefield.height * (minefield.width + 1) + 1;
    char* out_str = calloc(n_chars, sizeof(char));

    int ptr = 0;
    for (int i = 0; i < minefield.width; i++)
    {
        for (int j = 0; j < minefield.height; j++)
        {
            out_str[ptr++] = minefield.mines[i][j] ? 'X' : '-';
        }

        out_str[ptr++] = '\n';
    }

    return out_str;
}

void free_minefield(minefield_t minefield)
{
    // Frees all memory used by minefield_t instance
    free(minefield.mines);
}
