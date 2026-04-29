#ifndef HAB_GPS_H
#define HAB_GPS_H

#include <stdbool.h>
#include <stddef.h>

#include "hab_types.h"

#define HAB_GPS_LINE_BUFFER_LEN 128U

typedef struct {
    char line[HAB_GPS_LINE_BUFFER_LEN];
    size_t length;
} hab_gps_parser_t;

void hab_gps_parser_init(hab_gps_parser_t *parser);
bool hab_gps_parser_consume(hab_gps_parser_t *parser, char byte, hab_gps_data_t *gps_data);

#endif
