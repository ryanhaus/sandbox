#include <stdio.h>
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/can.h>
#include <zephyr/sys/byteorder.h>

const struct device *const can_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

struct k_thread rx_thread_data;
CAN_MSGQ_DEFINE(test_msgq, 2);

K_THREAD_STACK_DEFINE(rx_thread_stack, 512);

void rx_thread(void* arg1, void* arg2, void* arg3)
{
    const struct can_filter filter = {
        .flags = CAN_FILTER_IDE,
        .id = 0x12345,
        .mask = CAN_EXT_ID_MASK,
    };

    struct can_frame frame;

    int filter_id = can_add_rx_filter_msgq(can_dev, &test_msgq, &filter);
    printf("Filter ID: %d\n", filter_id);

    while (1)
    {
        k_msgq_get(&test_msgq, &frame, K_FOREVER);

        printf("Message received:\n\t");
        
        for (int i = 0; i < frame.dlc; i++)
        {
            printf("%02X", frame.data[i]);
        }

        printf("\n");
    }
}

int main(void)
{
    const struct can_frame test_frame = {
        .flags = CAN_FRAME_IDE,
        .id = 0x12345,
        .dlc = 2,
    };

    uint16_t counter = 0;
    k_tid_t rx_tid;

    if (!device_is_ready(can_dev))
    {
        printf("CAN device %s not ready\n", can_dev->name);
        return 0;
    }

    if (can_set_mode(can_dev, CAN_MODE_LOOPBACK))
    {
        printf("Cannot set CAN mode\n");
        return 0;
    }

    if (can_start(can_dev))
    {
        printf("Cannot start CAN controller\n");
        return 0;
    }

    rx_tid = k_thread_create(&rx_thread_data, rx_thread_stack,
                             K_THREAD_STACK_SIZEOF(rx_thread_stack),
                             rx_thread, NULL, NULL, NULL,
                             2, 0, K_NO_WAIT);

    if (!rx_tid)
    {
        printf("Cannot create RX thread\n");
        return 0;
    }

    while (1)
    {
        UNALIGNED_PUT(sys_cpu_to_be16(counter), (uint16_t*)&test_frame.data[0]);
        counter++;
        can_send(can_dev, &test_frame, K_MSEC(100), NULL, NULL);

        k_msleep(500);
    }


    return 0;
}
