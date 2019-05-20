#ifndef __CLOUD_TRAVERSE_H
#define __CLOUD_TRAVERSE_H

#include <stdbool.h>
#include <stdint.h>
#include "pdjson.h"
#include "playlist/track.h"

bool traverse_playlist(json_stream *playlist,  void *ctx, bool (*callback)(const Track_t *track, void *ctx));

#endif
