#include "hab_sd_fatfs.h"

#include <string.h>

bool hab_sd_fatfs_init(hab_sd_fatfs_t *storage, const char *mount_path, const char *log_path)
{
    FRESULT result = FR_OK;

    if (storage == NULL || mount_path == NULL || log_path == NULL) {
        return false;
    }

    memset(storage, 0, sizeof(*storage));

    result = f_mount(&storage->fs, mount_path, 1);
    if (result != FR_OK) {
        return false;
    }

    storage->mounted = true;

    result = f_open(&storage->file, log_path, FA_OPEN_APPEND | FA_WRITE);
    if (result != FR_OK) {
        return false;
    }

    storage->file_open = true;
    return true;
}

bool hab_sd_fatfs_append_line(hab_sd_fatfs_t *storage, const char *line)
{
    FRESULT result = FR_OK;
    UINT bytes_written = 0u;
    size_t line_length = 0u;

    if (storage == NULL || line == NULL || !storage->mounted || !storage->file_open) {
        return false;
    }

    line_length = strlen(line);
    result = f_write(&storage->file, line, (UINT)line_length, &bytes_written);
    if (result != FR_OK || bytes_written != (UINT)line_length) {
        return false;
    }

    result = f_write(&storage->file, "\r\n", 2u, &bytes_written);
    return result == FR_OK && bytes_written == 2u;
}

bool hab_sd_fatfs_flush(hab_sd_fatfs_t *storage)
{
    if (storage == NULL || !storage->file_open) {
        return false;
    }

    return f_sync(&storage->file) == FR_OK;
}
