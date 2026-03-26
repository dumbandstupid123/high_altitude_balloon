#ifdef HAB_HOST_SIM

#include <stdio.h>
#include <string.h>

#include "hab_app.h"
#include "hab_math.h"
#include "hab_platform.h"

static void hab_feed_demo_gps(hab_app_t *app, unsigned int tick)
{
    char body[96];
    char sentence[128];
    unsigned char checksum = 0u;
    float altitude_m = (tick < 120u) ? (20.0f * (float)tick) : (2400.0f - (15.0f * (float)(tick - 120u)));
    const double latitude = 34.123456;
    const double longitude = -117.123456;
    int lat_deg = (int)latitude;
    int lon_deg = (int)(-longitude);
    double lat_min = (latitude - lat_deg) * 60.0;
    double lon_min = ((-longitude) - lon_deg) * 60.0;
    size_t i = 0u;

    snprintf(
        body,
        sizeof(body),
        "GPGGA,120000.00,%02d%07.4f,N,%03d%07.4f,W,1,10,1.0,%.1f,M,0.0,M,,",
        lat_deg,
        lat_min,
        lon_deg,
        lon_min,
        altitude_m);

    for (i = 0u; body[i] != '\0'; ++i) {
        checksum ^= (unsigned char)body[i];
    }

    snprintf(sentence, sizeof(sentence), "$%s*%02X\r\n", body, checksum);

    for (i = 0u; sentence[i] != '\0'; ++i) {
        hab_app_on_gps_byte(app, sentence[i]);
    }
}

int main(void)
{
    hab_platform_t platform;
    hab_app_t app;
    unsigned int tick = 0u;

    hab_platform_init_stub(&platform);
    hab_app_init(&app);

    if (!hab_app_bootstrap(&app, &platform)) {
        fprintf(stderr, "Failed to bootstrap HAB application.\n");
        return 1;
    }

    for (tick = 0u; tick < 180u; ++tick) {
        hab_feed_demo_gps(&app, tick);
        hab_app_on_tick(&app, &platform);
    }

    return 0;
}

#endif
