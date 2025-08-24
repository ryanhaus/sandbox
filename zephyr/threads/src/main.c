#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/sys/printk.h>
#include <string.h>

void printer_thread(const char* message, uint32_t sleep_time)
{
    while (1)
    {
        printk(message);
        k_msleep(sleep_time);
    }
}

K_THREAD_DEFINE(
    printer_thread0,
    1024,
    printer_thread,
    "Hello thread 0\n", 500, NULL,
    7, 0, 0
);

K_THREAD_DEFINE(
    printer_thread1,
    1024,
    printer_thread,
    "Hello thread 1\n", 300, NULL,
    7, 0, 0
);

K_THREAD_DEFINE(
    printer_thread2,
    1024,
    printer_thread,
    "Hello thread 2\n", 500, NULL,
    7, 0, 0
);
