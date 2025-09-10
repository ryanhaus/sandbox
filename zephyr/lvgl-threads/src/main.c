#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <lvgl.h>
#include <math.h>
#include <stdio.h>

#include "sensor.h"
#include "custom_scale.h"

volatile float sensor_value = 0.0f;

int main(void)
{
    const struct device* display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display_dev))
        return -ENODEV;

    /* setup LVGL */
    // set background color
    lv_obj_set_style_bg_color(
        lv_screen_active(),
        lv_color_hex(0xFFFFFF),
        LV_PART_MAIN
    );

    // create custom scale
    lv_obj_t *scale, *line, *label;
    setup_scale_custom(lv_screen_active(), &scale, &line, &label);


    /* LVGL main loop */
    display_blanking_off(display_dev);

    while (1)
    {
        // update scale
        update_scale_custom(scale, line, label, sensor_value);

        // timers
        lv_timer_handler();
        k_msleep(5);
    }

    return 0;
}

void update_sensor_val(void)
{
    while (1)
    {
        sensor_value = get_sensor_val();
        //printf("Updated sensor value: %.2f\t(uptime: %.2f s)\n", sensor_value, (float)k_uptime_get() / 1000.0f);
        k_msleep(20);
    }
}

K_THREAD_DEFINE(
    sensor_thread,
    1024,
    update_sensor_val,
    NULL, NULL, NULL,
    7, 0, 0
);
