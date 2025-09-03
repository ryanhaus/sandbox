#pragma once

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <stdio.h>

struct k_mutex mutex;
int mutex_val;
K_MUTEX_DEFINE(mutex);

void mutex_wr(void)
{
    while (1)
    {
        k_mutex_lock(&mutex, K_FOREVER);
        mutex_val++;
        k_mutex_unlock(&mutex);

        k_msleep(250);
    }
}

void mutex_rd(void)
{
    while (1)
    {
        k_mutex_lock(&mutex, K_FOREVER);
        printf("mutex_val = %i\n", mutex_val);
        k_mutex_unlock(&mutex);

        k_msleep(100);
    }
}

K_THREAD_DEFINE(
    mutex_rd_thread,
    1024,
    mutex_rd,
    NULL, NULL, NULL,
    7, 0, 0
);

K_THREAD_DEFINE(
    mutex_wr_thread,
    1024,
    mutex_wr,
    NULL, NULL, NULL,
    7, 0, 0
);
