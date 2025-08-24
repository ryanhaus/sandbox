#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/display.h>
#include <zephyr/kernel.h>

#include <lvgl.h>

void setup_gui(void)
{
    // set background color
    lv_obj_set_style_bg_color(
        lv_screen_active(),
        lv_color_hex(0xFF0000),
        LV_PART_MAIN
    );

    // create label
    lv_obj_t* label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello");

    lv_obj_set_style_text_color(
        lv_screen_active(),
        lv_color_hex(0xFFFFFF),
        LV_PART_MAIN
    );

    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

int main(void)
{
    const struct device* display_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_display));

    if (!device_is_ready(display_dev))
        return -ENODEV;

    setup_gui();

    lv_timer_handler();
    display_blanking_off(display_dev);

    while (1)
    {
        uint32_t sleep_ms = lv_timer_handler();

        k_msleep(MIN(sleep_ms, INT32_MAX));
    }

    return 0;
}
