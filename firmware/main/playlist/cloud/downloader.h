#ifndef __CLOUD_DOWNLOADER_H
#define __CLOUD_DOWNLOADER_H

#include <stdbool.h>
#include <stdint.h>
#include "util/tempfile.h"

bool cloud_downloader_start(Tempfile_t *tmp_playlist, uint32_t sequence);
bool cloud_downloader_is_running();
bool cloud_downloader_stop();

#endif
