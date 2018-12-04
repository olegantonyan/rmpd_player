#include "audio/vs1011.h"
#include "audio/player.h"

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"
#include "freertos/event_groups.h"
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

static player_state_t state = STOPPED;

static QueueHandle_t queue = NULL;
static SemaphoreHandle_t state_mutex = NULL;

const EventBits_t STATE_CHANGED_BIT = BIT0;
static EventGroupHandle_t event_group;

static void player_thread(void * args);
static size_t file_size(FILE *f);
static bool play(const char *fname);
static void set_state(player_state_t new_state);
static player_state_t get_state();
static bool wait_for_state(player_state_t desired_state, TickType_t ticks);

bool player_start(const char *fname) {
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
  return xQueueSend(queue, &m, portMAX_DELAY) == pdTRUE;
}

bool player_start_blocking(const char *fname) { // this should probably be removed with more sophistacated scheduler
  if (!player_start(fname)) {
    return false;
  }
  if (!wait_for_state(PLAYING, portMAX_DELAY)) {
    return false;
  }
  return wait_for_state(STOPPED, portMAX_DELAY);
}

bool player_stop() {
  ESP_LOGD(TAG, "stopping");
  vs1011_stop();
  return wait_for_state(STOPPED, portMAX_DELAY);
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
  state_mutex = xSemaphoreCreateMutex();
  if (state_mutex == NULL) {
    ESP_LOGE(TAG, "cannot create mutex");
    return false;
  }
  event_group = xEventGroupCreate();
    if (event_group == NULL) {
      ESP_LOGE(TAG, "cannot create event group");
      return false;
  }

  return xTaskCreate(player_thread, "player", 4096, NULL, 5, NULL) == pdPASS;
}

static size_t file_size(FILE *f) {
  fseek(f, 0, SEEK_END);
  size_t sz = ftell(f);
  fseek(f, 0, SEEK_SET);
  return sz;
}

static void player_thread(void * args) {
  vTaskDelay(100 / portTICK_PERIOD_MS); // wait 100ms in MUTE state (@see app note)
  vs1011_mute(false);
  while(true) {
    player_message_t message;
    if(xQueueReceive(queue, &message, portMAX_DELAY)) {
      switch(message.type) {
        case PLAYER_START:
          set_state(PLAYING);
          play(message.filename);
          set_state(STOPPED);
          break;
        default:
          break;
      }
      if (message.filename != NULL) {
        free(message.filename);
      }
    }
  }
}

static bool play(const char *fname) {
  if (fname == NULL) {
    ESP_LOGE(TAG, "null filename");
    return false;
  }
  ESP_LOGI(TAG, "start playing file '%s'", fname);
  FILE *f = fopen(fname, "rb");
  if (f == NULL) {
    ESP_LOGE(TAG, "failed to open file '%s' for reading", fname);
    return false;
  }
  vs1011_play(f);
  ESP_LOGI(TAG, "end playing file '%s'", fname);
  fclose(f);
  return true;
}

static void set_state(player_state_t new_state) {
  player_state_t old_state = get_state();
  xSemaphoreTake(state_mutex, portMAX_DELAY);
  state = new_state;
  xSemaphoreGive(state_mutex);
  if (old_state != new_state) {
    xEventGroupSetBits(event_group, STATE_CHANGED_BIT);
  }
}

static player_state_t get_state() {
  xSemaphoreTake(state_mutex, portMAX_DELAY);
  player_state_t result = state;
  xSemaphoreGive(state_mutex);
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
