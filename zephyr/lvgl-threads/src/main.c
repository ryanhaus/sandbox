#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>
#include <lvgl.h>
#include <stdio.h>
#include "sensor.h"

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

    // create label
    lv_obj_t* label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, ""); 

    lv_obj_set_style_text_color(
        lv_screen_active(),
        lv_color_hex(0x000000),
        LV_PART_MAIN
    );

    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    /* LVGL main loop */
    lv_timer_handler();
    display_blanking_off(display_dev);

    while (1)
    {
        char label_str[32];
        snprintf(label_str, sizeof(label_str), "Sensor Value: %.2f", sensor_value);

        lv_label_set_text(label, label_str);

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
