#pragma once

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdio.h>

volatile uint32_t volatile_val = 0;

void volatile_wr(void)
{
    while (1)
    {
        volatile_val += 1;
        k_msleep(250);
    }
}

void volatile_rd(void)
{
    while(1)
    {
        printf("volatile_val = %i\n", volatile_val);
        k_msleep(100);
    }
}

K_THREAD_DEFINE(
    volatile_rd_thread,
    1024,
    volatile_rd,
    NULL, NULL, NULL,
    7, 0, 0
);

K_THREAD_DEFINE(
    volatile_wr_thread,
    1024,
    volatile_wr,
    NULL, NULL, NULL,
    7, 0, 0
);
