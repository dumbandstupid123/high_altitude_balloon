#ifndef HAB_SD_FATFS_H
#define HAB_SD_FATFS_H

#include <stdbool.h>

#include "ff.h"

typedef struct {
    FATFS fs;
    FIL file;
    bool mounted;
    bool file_open;
} hab_sd_fatfs_t;

bool hab_sd_fatfs_init(hab_sd_fatfs_t *storage, const char *mount_path, const char *log_path);
bool hab_sd_fatfs_append_line(hab_sd_fatfs_t *storage, const char *line);
bool hab_sd_fatfs_flush(hab_sd_fatfs_t *storage);

#endif
