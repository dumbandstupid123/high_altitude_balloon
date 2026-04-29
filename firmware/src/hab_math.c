#include "hab_math.h"

#include <math.h>

float hab_pressure_altitude_m(float pressure_pa, float reference_pressure_pa)
{
    if (pressure_pa <= 0.0f || reference_pressure_pa <= 0.0f) {
        return 0.0f;
    }

    return 44330.0f * (1.0f - powf(pressure_pa / reference_pressure_pa, 0.19029495f));
}

float hab_pressure_from_altitude_m(float altitude_m, float reference_pressure_pa)
{
    if (reference_pressure_pa <= 0.0f) {
        return 0.0f;
    }

    return reference_pressure_pa * powf(1.0f - (altitude_m / 44330.0f), 5.255f);
}

float hab_low_pass_filter(float previous, float sample, float alpha)
{
    if (alpha <= 0.0f) {
        return previous;
    }

    if (alpha >= 1.0f) {
        return sample;
    }

    return previous + (alpha * (sample - previous));
}
