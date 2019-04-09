#ifndef __FILE_DOWNLOAD_H
#define __FILE_DOWNLOAD_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

int file_download_start(const char *url, const char *download_path, size_t buffer_size);

#endif
