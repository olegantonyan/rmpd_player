#ifndef __STREAM_PLAYLIST_H
#define __STREAM_PLAYLIST_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

bool stream_playlist_parse_file(const char *fname, char *url, size_t url_max_length);
bool stream_playlist_is_stream(const char *fname);

#endif
