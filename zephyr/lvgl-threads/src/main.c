#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>
#include <lvgl.h>
#include <stdio.h>
#include "sensor.h"

#define M_PI 3.14159265358979323846f

volatile float sensor_value = 0.0f;

int main(void)
{
    const struct device* display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display_dev))
        return -ENODEV;

    /* setup LVGL */
    const int SCREEN_WIDTH = lv_obj_get_width(lv_screen_active());
    const int SCREEN_HEIGHT = lv_obj_get_height(lv_screen_active());

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

    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);

    // create line
    const int LINE_BASE_X = SCREEN_WIDTH / 2;
    const int LINE_BASE_Y = SCREEN_HEIGHT - 30;
    const int LINE_LENGTH = SCREEN_WIDTH / 2 - 10;

    lv_point_precise_t line_pts[] = {
        { LINE_BASE_X, LINE_BASE_Y },
        { LINE_BASE_X, LINE_BASE_Y },
    };

    lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 8);
    lv_style_set_line_color(&style_line, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_line_rounded(&style_line, true);

    lv_obj_t* line = lv_line_create(lv_screen_active());
    lv_line_set_points(line, line_pts, 2);
    lv_obj_add_style(line, &style_line, 0);

    /* LVGL main loop */
    lv_timer_handler();
    display_blanking_off(display_dev);

    while (1)
    {
        // update line position
        lv_point_precise_t line_pts[] = {
            { LINE_BASE_X, LINE_BASE_Y },
            {
                LINE_BASE_X - LINE_LENGTH * cosf(sensor_value * M_PI / 10.0f),
                LINE_BASE_Y - LINE_LENGTH * sinf(sensor_value * M_PI / 10.0f)
            },
        };

        lv_line_set_points(line, line_pts, 2);

        // update label text
        char label_str[32];
        snprintf(label_str, sizeof(label_str), "%.1f", (double)sensor_value);

        lv_label_set_text(label, label_str);

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
