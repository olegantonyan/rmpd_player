#include "audio/mp3.h"
#include "helix/mp3dec.h"

extern const char mp3_data[];
#define MP3_SIZE	687348

static MP3FrameInfo frame_info;
static HMP3Decoder decoder;

void mp3_init() {
  decoder = MP3InitDecoder();
}
