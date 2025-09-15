#include <stdio.h>
#include <zephyr/kernel.h>

int main(void)
{
    while (1)
    {
        printf("Tick\n");

        k_msleep(250);
    }

    return 0;
}
