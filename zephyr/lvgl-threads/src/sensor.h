#include <zephyr/kernel.h>
#include <math.h>

float get_sensor_val(void)
{
    float uptime_s = (float)k_uptime_get() / 1000.0f;
    return sinf(uptime_s);
}
