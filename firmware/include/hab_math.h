#ifndef HAB_MATH_H
#define HAB_MATH_H

float hab_pressure_altitude_m(float pressure_pa, float reference_pressure_pa);
float hab_pressure_from_altitude_m(float altitude_m, float reference_pressure_pa);
float hab_low_pass_filter(float previous, float sample, float alpha);

#endif
