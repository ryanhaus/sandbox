#pragma once
#include <zephyr/kernel.h>
#include <math.h>

float get_other_sensor_val(void)
{
    return fmodf((float)k_uptime_get() / 100.0f, 25.0f);
}
