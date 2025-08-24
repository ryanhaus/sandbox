#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>

#include <lvgl.h>

int main(void)
{
    const struct device *display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display_dev))
        return -ENODEV;

    // initialize_gui();

    lv_timer_handler();
    display_blanking_off(display_dev);

    while (1)
    {
        uint32_t sleep_ms = lv_timer_handler();

        k_msleep(MIN(sleep_ms, INT32_MAX));
    }

    return 0;
}
