#include <stdbool.h>

typedef struct {
    int width;
    int height;
    bool** mines;
} minefield_t;

minefield_t create_empty_minefield(int width, int height);
minefield_t create_filled_minefield(int width, int height, int n_mines);
char* get_minefield_str(minefield_t minefield);
void free_minefield(minefield_t minefield);
