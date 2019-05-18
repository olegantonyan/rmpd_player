#ifndef __CLOUD_SCHEDULER_H
#define __CLOUD_SCHEDULER_H

#include <stdbool.h>
#include <stdint.h>
#include "storage/sd.h"

#define CLOUD_SCHEDULER_PLAYLIST_FILENAME "playlist.json"
#define CLOUD_SCHEDULER_FILES_PATH    (STORAGE_SD_MOUNTPOINT "/cloud/mediafiles")
#define CLOUD_SCHEDULER_PLAYLIST_PATH (STORAGE_SD_MOUNTPOINT "/cloud/mediafiles/" CLOUD_SCHEDULER_PLAYLIST_FILENAME)

bool cloud_scheduler_is_enabled();
bool cloud_scheduler_init();
bool cloud_scheduler_deinit();

#endif
