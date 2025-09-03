#include <zephyr/kernel.h>
#include <math.h>

float get_sensor_val(void)
{
    float uptime_s = (float)k_uptime_get() / 1000.0f;
    return 10.0f * (0.5f + 0.5f * sinf(uptime_s));
}

float get_other_sensor_val(void)
{
    float uptime_s = (float)k_uptime_get() / 1000.0f;

    return 100.0f * (0.5f + 0.5f * cosf(uptime_s / 3.0f));
}
