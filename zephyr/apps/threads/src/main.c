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

K_THREAD_STACK_DEFINE(thread_stack_area, 1024);

int main(void)
{
    struct k_thread thread_data;

    k_thread_create(
        &thread_data,
        thread_stack_area,
        K_THREAD_STACK_SIZEOF(thread_stack_area),
        printer_thread,
        "Hello dynamic thread\n", 750, NULL,
        7, 0, K_NO_WAIT
    );
    
    printer_thread("Hello main thread\n", 1000);

    return 0;
}

K_THREAD_DEFINE(
    printer_thread1,
    1024,
    printer_thread,
    "Hello static thread 1\n", 500, NULL,
    7, 0, 0
);

K_THREAD_DEFINE(
    printer_thread2,
    1024,
    printer_thread,
    "Hello static thread 2\n", 300, NULL,
    7, 0, 0
);
