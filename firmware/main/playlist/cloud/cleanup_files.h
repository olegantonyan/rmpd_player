#ifndef __CLEANUP_FILES_H
#define __CLEANUP_FILES_H

#include <stdbool.h>
#include <stdint.h>

bool cloud_cleanup_files_start();
bool cloud_cleanup_files_is_running();
bool cloud_cleanup_files_stop();

#endif
