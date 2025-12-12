#include <stdio.h>
#include <stdlib.h>

#include "minefield.h"

int main()
{
    // Create easy minefield (9x9, 10 mines)
    minefield_t minefield = create_filled_minefield(9, 9, 10);

    // Print out
    printf("%s", get_minefield_str(minefield));

    return 0;
}
