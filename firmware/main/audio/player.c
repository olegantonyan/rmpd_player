#include "audio/vs1011.h"
#include "audio/player.h"
#include "config/config.h"
#include "audio/stream.h"
#include "audio/stream_playlist.h"
#include "wifi/wifi.h"

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "util/strings.h"
#include "util/files.h"
#include "esp_system.h"
#include "esp_log.h"
#include "audio/stream_playlist.h"
#include "remote/control.h"
#include "util/read_ahead.h"

static const char *TAG = "player";

typedef enum {
  PLAYING = 100,
  STOPPED
} player_state_t;

typedef struct {
  char *filename; // XXX: dynamically allocated
  void (*error_callback)();
} player_message_t;

typedef struct {
  bool success;
} player_result_t;

static struct {
  player_state_t state;
  SemaphoreHandle_t mutex;
  char *now_playing; // XXX: points to dynamically allocated string from player_message_t
  uint32_t pos_total;
  uint32_t pos_current;
  uint16_t pos_seconds;
} state = { STOPPED, NULL, NULL, 0, 0, 0 };

static QueueHandle_t queue = NULL;
static QueueHandle_t back_queue = NULL;

static const EventBits_t STATE_CHANGED_BIT = BIT0;
static const EventBits_t PLAYER_STOP_BIT = BIT1;
static const EventBits_t PLAYER_STOPPED_BIT = BIT2;
static EventGroupHandle_t event_group = NULL;

static void player_thread(void * args);
static bool play(player_message_t *pm);
static bool play_stream(player_message_t *pm);
static bool play_file(player_message_t *pm);
static void set_state(player_state_t new_state);
static player_state_t get_state();
static bool wait_for_state(player_state_t desired_state, TickType_t ticks);
static void set_now_playing(char *str);
static void vs1011_callback(audio_info_t ai);
static size_t file_read_func(uint8_t *buffer, size_t buffer_size, void *ctx);
static size_t stream_read_func(uint8_t *buffer, size_t buffer_size, void *ctx);

bool player_start(const char *fname, void (*error_callback)()) {
  ESP_LOGD(TAG, "player start");
  if (get_state() == PLAYING) {
    player_stop();
  }
  ESP_LOGD(TAG, "starting %s", fname);
  if (strlen(fname) > 1024) {
    ESP_LOGE(TAG, "too long filename %s", fname);
    return false;
  }
  player_message_t m;
  m.filename = malloc(strlen(fname) + 1);
  if (m.filename == NULL) {
    ESP_LOGE(TAG, "malloc failed");
    return false;
  }
  m.filename[0] = '\0';
  strcpy(m.filename, fname);
  m.error_callback = error_callback;

  xEventGroupClearBits(event_group, PLAYER_STOP_BIT | PLAYER_STOPPED_BIT);

  BaseType_t result = xQueueSend(queue, &m, portMAX_DELAY);
  if (result != pdTRUE) {
    ESP_LOGE(TAG, "queue send failed");
    return false;
  }

  player_result_t pr;
  result = xQueueReceive(back_queue, &pr, portMAX_DELAY);
  if (result != pdTRUE) {
    ESP_LOGE(TAG, "queue receive failed");
    return false;
  }
  return pr.success;
}

bool player_stop() {
  ESP_LOGD(TAG, "stopping");
  if (get_state() == STOPPED) {
    return true;
  }
  xEventGroupSetBits(event_group, PLAYER_STOP_BIT);
  bool ok = wait_for_state(STOPPED, portMAX_DELAY);
  xEventGroupClearBits(event_group, PLAYER_STOP_BIT);
  return ok;
}

bool player_get_now_playing(char *buffer, size_t length) {
  if (state.mutex == NULL) {
    return false;
  }

  bool result = false;
  xSemaphoreTake(state.mutex, portMAX_DELAY);
  if (state.now_playing != NULL) {
    buffer[0] = '\0';
    strncpy(buffer, state.now_playing, length);
    result = true;
  }
  xSemaphoreGive(state.mutex);
  return result;
}

uint8_t player_get_position_percents() {
  if (state.pos_total == 0) {
    return 0;
  }
  return (uint8_t)((state.pos_current / (float)state.pos_total) * 100.0);
}

uint16_t player_get_position_seconds() {
  return state.pos_seconds;
}

bool player_init() {
  bool vs_ok = vs1011_init();
  if (!vs_ok) {
    ESP_LOGE(TAG, "cannot initialize audio driver");
    return false;
  }

  queue = xQueueCreate(1, sizeof(player_message_t));
  if (queue == NULL) {
    ESP_LOGE(TAG, "cannot create queue");
    return false;
  }
  back_queue = xQueueCreate(1, sizeof(player_result_t));
  if (back_queue == NULL) {
    ESP_LOGE(TAG, "cannot create back queue");
    return false;
  }
  state.mutex = xSemaphoreCreateMutex();
  if (state.mutex == NULL) {
    ESP_LOGE(TAG, "cannot create mutex");
    return false;
  }
  event_group = xEventGroupCreate();
  if (event_group == NULL) {
    ESP_LOGE(TAG, "cannot create event group");
    return false;
  }

  player_set_volume(config_volume());
  player_set_bass_freqlimit(config_bass_freqlimit());
  player_set_bass_amplitude(config_bass_amplitude());
  player_set_treble_freqlimit(config_treble_freqlimit());
  player_set_treble_amplitude(config_treble_amplitude());

  return xTaskCreate(player_thread, TAG, 4096, NULL, 15, NULL) == pdPASS;
}

void player_set_volume(uint8_t percents) {
  if(percents > 100) {
    percents = 100;
  }
  vs1011_set_volume(percents);
  if (percents != config_volume()) {
    config_save_volume(percents);
  }
}

void player_set_bass_freqlimit(uint8_t value) { // 2-15          //  20Hz - 150Hz
  if (value < 2) {
    value = 2;
  }
  if (value > 15) {
    value = 15;
  }
  vs1011_set_bass_freqlimit(value);
  if (value != config_bass_freqlimit()) {
    config_save_bass_freqlimit(value);
  }
}

void player_set_bass_amplitude(uint8_t value) { // 0-15, 0=off   //  0dB  - 15dB
  if (value > 15) {
    value = 15;
  }
  vs1011_set_bass_amplitude(value);
  if (value != config_bass_amplitude()) {
    config_save_bass_amplitude(value);
  }
}

void player_set_treble_freqlimit(uint8_t value) { // 0-15        //  0KHz - 15KHz
  if (value > 15) {
    value = 15;
  }
  vs1011_set_treble_freqlimit(value);
  if (value != config_treble_freqlimit()) {
    config_save_treble_freqlimit(value);
  }
}

void player_set_treble_amplitude(int8_t value) { // -8-7, 0=off  // -12dB - 10.5dB
  if (value < -8) {
    value = -8;
  }
  if (value > 7) {
    value = 7;
  }
  vs1011_set_treble_amplitude(value);
  if (value != config_treble_amplitude()) {
    config_save_treble_amplitude(value);
  }
}

static void player_thread(void * args) {
  vTaskDelay(pdMS_TO_TICKS(100)); // wait 100ms in MUTE state (@see vs1011 app note)
  vs1011_transient_mute(false);
  while(true) {
    player_message_t message;
    if(xQueueReceive(queue, &message, portMAX_DELAY)) {
      set_now_playing(message.filename);
      set_state(PLAYING);
      player_result_t result;
      result.success = play(&message);
      set_now_playing(NULL);
      xQueueSend(back_queue, &result, pdMS_TO_TICKS(1000));
      set_state(STOPPED);
      if (message.filename != NULL) {
        free(message.filename);
      }
    }
  }
}

static bool play(player_message_t *pm) {
  if (pm->filename == NULL) {
    ESP_LOGE(TAG, "null filename");
    return false;
  }

  bool result = false;
  ESP_LOGI(TAG, "start playing file '%s'", pm->filename);
  if (stream_playlist_is_stream(pm->filename)) {
    result = play_stream(pm);
  } else {
    result = play_file(pm);
  }
  ESP_LOGI(TAG, "end playing file '%s'", pm->filename);

  return result;
}

static bool play_stream(player_message_t *pm) {
  stream_t stream;
  char url[512] = { 0 };

  uint8_t retries = config_stream_retries();
  bool infinite = retries == 0;
  do {
    if (!wifi_is_connected()) {
      ESP_LOGW(TAG, "no network - ignoring stream start");
      if (pm->error_callback != NULL) {
        pm->error_callback();
      }
      return false;
    }
    memset(url, 0, sizeof(url));
    if (!stream_playlist_parse_file(pm->filename, url, sizeof(url))) {
      ESP_LOGE(TAG, "error parsing playlist file '%s'", pm->filename);
      if (pm->error_callback != NULL) {
        pm->error_callback();
      }
      return false;
    }

    if (!stream_start(url, VS1011_BUFFER_SIZE, &stream)) {
      ESP_LOGE(TAG, "failed to start a stream '%s'", url);
      if (pm->error_callback != NULL) {
        pm->error_callback();
      }
      continue;
    }
    vs1011_play(stream_read_func, 0, &stream, vs1011_callback);

    if (xEventGroupGetBits(event_group) & PLAYER_STOPPED_BIT) {
      xEventGroupClearBits(event_group, PLAYER_STOPPED_BIT);
      ESP_LOGD(TAG, "stopped by player stop");
      stream_stop(&stream);
      return true;
    }

    if (infinite) {
      ESP_LOGI(TAG, "stopped by eos, retry (infinite)");
    } else {
      ESP_LOGI(TAG, "stopped by eos, retries remaining %d", retries);
    }
    stream_stop(&stream);
    if (pm->error_callback != NULL) {
      pm->error_callback();
    }
  } while (infinite || --retries > 0);

  return infinite || retries > 0;
}

static bool play_file(player_message_t *pm) {
  FILE *f = fopen(pm->filename, "rb");
  if (f == NULL) {
    ESP_LOGE(TAG, "failed to open file '%s' for reading", pm->filename);
    return false;
  }

  size_t fsize = file_size(f);
  ReadAhead_t *ra = read_ahead_init(f, VS1011_BUFFER_SIZE);
  vs1011_play(file_read_func, fsize, (void *)ra, vs1011_callback);
  fclose(f);
  read_ahead_deinit(ra);
  return true;
}

static void set_now_playing(char *str) {
  size_t i = 0;
  if (str != NULL) {
    i = strlen(str);
    do {
      i--;
    } while (str[i] != '/' && i > 0);
  }
  xSemaphoreTake(state.mutex, portMAX_DELAY);
  if (i != 0) {
    state.now_playing = str + i + 1;
  } else {
    state.now_playing = str;
  }
  xSemaphoreGive(state.mutex);
  remote_control_force_cycle(); // kick it to force send now_playing
}

static void set_state(player_state_t new_state) {
  player_state_t old_state = get_state();
  xSemaphoreTake(state.mutex, portMAX_DELAY);
  state.state = new_state;
  xSemaphoreGive(state.mutex);
  if (old_state != new_state) {
    xEventGroupSetBits(event_group, STATE_CHANGED_BIT);
  }
}

static player_state_t get_state() {
  xSemaphoreTake(state.mutex, portMAX_DELAY);
  player_state_t result = state.state;
  xSemaphoreGive(state.mutex);
  return result;
}

static bool wait_for_state(player_state_t desired_state, TickType_t ticks) {
  if (get_state() == desired_state) {
    return true;
  }
  xEventGroupClearBits(event_group, STATE_CHANGED_BIT);
  xEventGroupWaitBits(event_group, STATE_CHANGED_BIT, pdTRUE, pdFALSE, ticks);
  return get_state() == desired_state;
}

static void vs1011_callback(audio_info_t ai) {
  state.pos_total = ai.total;
  state.pos_current = ai.position;
  state.pos_seconds = ai.decode_time;
}

static size_t file_read_func(uint8_t *buffer, size_t buffer_size, void *ctx) {
  if (xEventGroupGetBits(event_group) & PLAYER_STOP_BIT) {
    xEventGroupClearBits(event_group, PLAYER_STOP_BIT);
    xEventGroupSetBits(event_group, PLAYER_STOPPED_BIT);
    return 0;
  }
  return read_ahead_next((ReadAhead_t *)ctx, buffer, buffer_size);
}

static size_t stream_read_func(uint8_t *buffer, size_t buffer_size, void *ctx) {
  if (xEventGroupGetBits(event_group) & PLAYER_STOP_BIT) {
    xEventGroupClearBits(event_group, PLAYER_STOP_BIT);
    xEventGroupSetBits(event_group, PLAYER_STOPPED_BIT);
    return 0;
  }
  return stream_read((stream_t *)ctx, buffer, buffer_size);
}
