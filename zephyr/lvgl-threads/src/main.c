#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <lvgl.h>
#include <math.h>
#include <stdio.h>
#include "sensor.h"

#define M_PI 3.14159265358979323846f
#define M_2PI (2.0f * M_PI)

volatile float sensor_value = 0.0f;

void update_scale_line_val(lv_obj_t* scale, lv_obj_t* line, float val)
{
    const static int LINE_LEN = 60;
    const static int LINE_OFFSET_X = 480 / 2;
    const static int LINE_OFFSET_Y = 272 / 2;

    int min = lv_scale_get_range_min_value(scale),
        max = lv_scale_get_range_max_value(scale),
        range = max - min,
        angle_range = lv_scale_get_angle_range(scale),
        rot = lv_scale_get_rotation(scale);

    static lv_point_precise_t line_pts[2];

    line_pts[0].x = LINE_OFFSET_X;
    line_pts[0].y = LINE_OFFSET_Y;

    float angle_mult = M_2PI * (float)angle_range / 360.0f,
          angle_offset = M_2PI * (float)rot / 360.0f;
    line_pts[1].x = LINE_OFFSET_X + LINE_LEN * cosf(angle_mult * val / range + angle_offset);
    line_pts[1].y = LINE_OFFSET_Y + LINE_LEN * sinf(angle_mult * val / range + angle_offset);

    lv_line_set_points(line, line_pts, 2);
    lv_obj_move_foreground(line);
}

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

    lv_obj_set_style_text_font(
        label,
        &lv_font_montserrat_20,
        LV_PART_MAIN
    ); 
    
    lv_obj_set_style_text_color(
        lv_screen_active(),
        lv_color_hex(0x000000),
        LV_PART_MAIN
    );

    lv_obj_align(label, LV_ALIGN_BOTTOM_MID, 0, 0);

    // create scale
    lv_obj_t* scale = lv_scale_create(lv_screen_active());

    lv_obj_set_size(scale, 150, 150);
    lv_scale_set_mode(scale, LV_SCALE_MODE_ROUND_INNER);

    lv_obj_set_style_text_font(
        scale,
        &lv_font_montserrat_14,
        LV_PART_MAIN
    ); 

    lv_obj_set_style_bg_opa(scale, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(scale, lv_color_hex(0xDDDDDD), 0);
    lv_obj_set_style_radius(scale, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_clip_corner(scale, true, 0);
    lv_obj_align(scale, LV_ALIGN_LEFT_MID, LV_PCT(2), 0);

    lv_scale_set_label_show(scale, true);

    lv_scale_set_total_tick_count(scale, 26);
    lv_scale_set_major_tick_every(scale, 5);

    lv_obj_set_style_length(scale, 5, LV_PART_ITEMS);
    lv_obj_set_style_length(scale, 10, LV_PART_INDICATOR);
    lv_scale_set_range(scale, 0, 25);

    lv_scale_set_angle_range(scale, 270);
    lv_scale_set_rotation(scale, 135);
    lv_obj_center(scale);

    // create scale line
    lv_obj_t* line = lv_line_create(lv_screen_active());
    lv_obj_set_style_line_color(line, lv_color_hex(0xFF0000), LV_PART_MAIN);
    lv_obj_set_style_line_width(line, 6, LV_PART_MAIN);
    lv_obj_set_style_line_rounded(line, true, LV_PART_MAIN);

    /* LVGL main loop */
    display_blanking_off(display_dev);

    while (1)
    {
        // update label text
        char label_str[32];
        snprintf(label_str, sizeof(label_str), "%.1f", (double)sensor_value);

        lv_label_set_text(label, label_str);
        
        // update line position
        update_scale_line_val(scale, line, sensor_value);

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


void blinky(void)
{
    struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

    if (!gpio_is_ready_dt(&led))
		return;

    gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);

	while (1)
    {
		gpio_pin_toggle_dt(&led);
		k_msleep(500);
	}
}

K_THREAD_DEFINE(
    blinky_thread,
    1024,
    blinky,
    NULL, NULL, NULL,
    7, 0, 0
);
