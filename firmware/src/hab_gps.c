#include "hab_gps.h"

#include <stdlib.h>
#include <string.h>

static bool hab_nmea_checksum_valid(const char *sentence)
{
    const char *start = sentence;
    const char *asterisk = strchr(sentence, '*');
    unsigned char checksum = 0;

    if (*start == '$') {
        start++;
    }

    if (asterisk == NULL) {
        return true;
    }

    while (start < asterisk) {
        checksum ^= (unsigned char)(*start);
        start++;
    }

    return (unsigned char)strtoul(asterisk + 1, NULL, 16) == checksum;
}

static double hab_parse_lat_lon(const char *field, char hemisphere)
{
    char *end_ptr = NULL;
    double raw = strtod(field, &end_ptr);
    int degrees = 0;
    double minutes = 0.0;

    if (field == end_ptr) {
        return 0.0;
    }

    degrees = (int)(raw / 100.0);
    minutes = raw - (degrees * 100.0);
    raw = degrees + (minutes / 60.0);

    if (hemisphere == 'S' || hemisphere == 'W') {
        raw = -raw;
    }

    return raw;
}

static void hab_copy_time_string(hab_gps_data_t *gps, const char *field)
{
    size_t i = 0;

    if (field[0] == '\0') {
        return;
    }

    while (field[i] != '\0' && i < sizeof(gps->utc_time) - 1u) {
        gps->utc_time[i] = field[i];
        i++;
    }

    gps->utc_time[i] = '\0';
    gps->valid_time = true;
}

static bool hab_parse_gga(char *body, hab_gps_data_t *gps)
{
    char *fields[16] = {0};
    size_t field_count = 0;
    char *token = strtok(body, ",");

    while (token != NULL && field_count < (sizeof(fields) / sizeof(fields[0]))) {
        fields[field_count++] = token;
        token = strtok(NULL, ",");
    }

    if (field_count < 10u) {
        return false;
    }

    hab_copy_time_string(gps, fields[1]);
    gps->latitude_deg = hab_parse_lat_lon(fields[2], fields[3][0]);
    gps->longitude_deg = hab_parse_lat_lon(fields[4], fields[5][0]);
    gps->valid_fix = fields[6][0] > '0';
    gps->satellites = (uint8_t)strtoul(fields[7], NULL, 10);
    gps->altitude_m = (float)strtod(fields[9], NULL);
    return true;
}

static bool hab_parse_rmc(char *body, hab_gps_data_t *gps)
{
    char *fields[16] = {0};
    size_t field_count = 0;
    char *token = strtok(body, ",");

    while (token != NULL && field_count < (sizeof(fields) / sizeof(fields[0]))) {
        fields[field_count++] = token;
        token = strtok(NULL, ",");
    }

    if (field_count < 7u) {
        return false;
    }

    hab_copy_time_string(gps, fields[1]);
    gps->valid_fix = fields[2][0] == 'A';
    gps->latitude_deg = hab_parse_lat_lon(fields[3], fields[4][0]);
    gps->longitude_deg = hab_parse_lat_lon(fields[5], fields[6][0]);
    return true;
}

void hab_gps_parser_init(hab_gps_parser_t *parser)
{
    parser->length = 0u;
    parser->line[0] = '\0';
}

bool hab_gps_parser_consume(hab_gps_parser_t *parser, char byte, hab_gps_data_t *gps_data)
{
    char scratch[HAB_GPS_LINE_BUFFER_LEN];

    if (byte == '\r') {
        return false;
    }

    if (byte != '\n') {
        if (parser->length < (HAB_GPS_LINE_BUFFER_LEN - 1u)) {
            parser->line[parser->length++] = byte;
            parser->line[parser->length] = '\0';
        } else {
            parser->length = 0u;
            parser->line[0] = '\0';
        }
        return false;
    }

    if (parser->length == 0u) {
        return false;
    }

    memcpy(scratch, parser->line, parser->length + 1u);
    parser->length = 0u;
    parser->line[0] = '\0';

    if (scratch[0] != '$' || !hab_nmea_checksum_valid(scratch)) {
        return false;
    }

    if (strncmp(scratch, "$GPGGA", 6) == 0 || strncmp(scratch, "$GNGGA", 6) == 0) {
        return hab_parse_gga(scratch, gps_data);
    }

    if (strncmp(scratch, "$GPRMC", 6) == 0 || strncmp(scratch, "$GNRMC", 6) == 0) {
        return hab_parse_rmc(scratch, gps_data);
    }

    return false;
}
