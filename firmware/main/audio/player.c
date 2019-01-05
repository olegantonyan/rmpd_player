#include "audio/vs1011.h"
#include "audio/player.h"
#include "config/config.h"
#include "audio/stream.h"

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
#include "util/files.h"
#include "esp_system.h"
#include "esp_log.h"

static const char *TAG = "player";

typedef enum {
  PLAYER_START = 100,
} player_message_type_t;

typedef enum {
  PLAYING = 100,
  STOPPED
} player_state_t;

typedef struct {
  player_message_type_t type;
  char *filename; // XXX: dynamically allocated
} player_message_t;

static struct {
  player_state_t state;
  SemaphoreHandle_t mutex;
  char *now_playing; // XXX: points to dynamically allocated string from player_message_t
  uint32_t pos_total;
  uint32_t pos_current;
} state = { STOPPED, NULL, NULL, 0, 0 };

static QueueHandle_t queue = NULL;

static const EventBits_t STATE_CHANGED_BIT = BIT0;
static const EventBits_t PLAYER_STOP_BIT = BIT1;
static EventGroupHandle_t event_group;

static void player_thread(void * args);
static bool play(const char *fname);
static void set_state(player_state_t new_state);
static player_state_t get_state();
static bool wait_for_state(player_state_t desired_state, TickType_t ticks);
static void set_now_playing(char *str);
static void vs1011_callback(uint32_t position, uint32_t total);
static size_t file_read_func(uint8_t *buffer, size_t buffer_size, void *ctx);
static size_t stream_read_func(uint8_t *buffer, size_t buffer_size, void *ctx);

bool player_start(const char *fname, bool async) {
  if (get_state() == PLAYING) {
    player_stop();
  }
  ESP_LOGD(TAG, "starting %s", fname);
  if (strlen(fname) > 1024) {
    ESP_LOGE(TAG, "too long filename %s", fname);
    return false;
  }
  player_message_t m;
  m.type = PLAYER_START;
  m.filename = malloc(strlen(fname) + 1);
  if (m.filename == NULL) {
    ESP_LOGE(TAG, "malloc failed");
    return false;
  }
  m.filename[0] = '\0';
  strcpy(m.filename, fname);

  BaseType_t result = xQueueSend(queue, &m, portMAX_DELAY);
  if (async) {
    return result == pdTRUE;
  }
  if (result != pdTRUE) {
    return false;
  }
  if (!wait_for_state(PLAYING, portMAX_DELAY)) {
    return false;
  }
  return wait_for_state(STOPPED, portMAX_DELAY);
}

bool player_stop() {
  ESP_LOGD(TAG, "stopping");
  if (get_state() == STOPPED) {
    return true;
  }
  //vs1011_stop();
  xEventGroupSetBits(event_group, PLAYER_STOP_BIT);
  return wait_for_state(STOPPED, portMAX_DELAY);
}

bool player_get_now_playing(char *buffer, size_t length) {
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

bool player_init() {
  bool vs_ok = vs1011_init();
  if (!vs_ok) {
    ESP_LOGE(TAG, "cannot initialize audio driver");
    return false;
  }

  queue = xQueueCreate(5, sizeof(player_message_t));
  if (queue == NULL) {
    ESP_LOGE(TAG, "cannot create queue");
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

  return xTaskCreate(player_thread, "player", 4096, NULL, 15, NULL) == pdPASS;
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
  vTaskDelay(100 / portTICK_PERIOD_MS); // wait 100ms in MUTE state (@see vs1011 app note)
  vs1011_transient_mute(false);
  while(true) {
    player_message_t message;
    if(xQueueReceive(queue, &message, portMAX_DELAY)) {
      switch(message.type) {
        case PLAYER_START:
          set_now_playing(message.filename);
          set_state(PLAYING);
          play(message.filename);
          set_state(STOPPED);
          set_now_playing(NULL);
          break;
        default:
          break;
      }
      if (message.filename != NULL) {
        free(message.filename);
        set_now_playing(NULL); // just in case
      }
    }
  }
}

static bool play(const char *fname) {
  stream_t stream;
  stream_start("http://us4.internet-radio.com:8258/", VS1011_BUFFER_SIZE, &stream);
  vs1011_play(stream_read_func, 0, &stream, vs1011_callback);
  stream_stop(&stream);

  /*if (fname == NULL) {
    ESP_LOGE(TAG, "null filename");
    return false;
  }
  ESP_LOGI(TAG, "start playing file '%s'", fname);
  FILE *f = fopen(fname, "rb");
  if (f == NULL) {
    ESP_LOGE(TAG, "failed to open file '%s' for reading", fname);
    return false;
  }
  vs1011_play(file_read_func, file_size(f), (void *)f, vs1011_callback);
  ESP_LOGI(TAG, "end playing file '%s'", fname);
  fclose(f);*/
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

static void vs1011_callback(uint32_t position, uint32_t total) {
  state.pos_total = total;
  state.pos_current = position;
}

static size_t file_read_func(uint8_t *buffer, size_t buffer_size, void *ctx) {
  if (xEventGroupGetBits(event_group) & PLAYER_STOP_BIT) {
    xEventGroupClearBits(event_group, PLAYER_STOP_BIT);
    return 0;
  }
  return fread(buffer, 1, buffer_size, (FILE *)ctx);
}

static size_t stream_read_func(uint8_t *buffer, size_t buffer_size, void *ctx) {
  if (xEventGroupGetBits(event_group) & PLAYER_STOP_BIT) {
    xEventGroupClearBits(event_group, PLAYER_STOP_BIT);
    return 0;
  }
  return stream_read((stream_t *)ctx, buffer, buffer_size);
}
