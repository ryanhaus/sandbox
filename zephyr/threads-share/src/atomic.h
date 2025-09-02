#pragma once

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdio.h>

atomic_t atomic_val; // default value: 0

void atomic_wr(void)
{
    while (1)
    {
        atomic_inc(&atomic_val);
        k_msleep(250);
    }
}

void atomic_rd(void)
{
    while (1)
    {
        printf("atomic_val = %i\n", (int)atomic_val);
        k_msleep(100);
    }
}

K_THREAD_DEFINE(
    atomic_rd_thread,
    1024,
    atomic_rd,
    NULL, NULL, NULL,
    7, 0, 0
);

K_THREAD_DEFINE(
    atomic_wr_thread,
    1024,
    atomic_wr,
    NULL, NULL, NULL,
    7, 0, 0
);
