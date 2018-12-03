#include "audio/vs1011.h"
#include "audio/player.h"

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include "storage/sd.h"

static const char *TAG = "player";

typedef enum {
  PLAYER_START = 100,
} player_message_type_t;

typedef struct {
  player_message_type_t type;
  char *filename; // XXX: dynamically allocated
} player_message_t;

const EventBits_t PLAYING_BIT = BIT0;

static QueueHandle_t queue = NULL;
static EventGroupHandle_t event_group;

static void player_thread(void * args);
static size_t file_size(FILE *f);
static bool play(const char *fname);

bool player_start(const char *fname) {
  if (xEventGroupGetBits(event_group) & PLAYING_BIT) {
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

bool player_stop() {
  ESP_LOGD(TAG, "stopping");
  vs1011_stop();
  return (xEventGroupWaitBits(event_group, PLAYING_BIT, pdTRUE, pdFALSE, portMAX_DELAY) & PLAYING_BIT) != PLAYING_BIT;
}

bool player_init() {
  bool vs_ok = vs1011_init();
  if (!vs_ok) {
    ESP_LOGE(TAG, "cannot initialize audio driver");
    return false;
  }
  //vs1011_mute(true);

  queue = xQueueCreate(5, sizeof(player_message_t));
  if (queue == NULL) {
    ESP_LOGE(TAG, "cannot create queue");
    return false;
  }
  event_group = xEventGroupCreate();
  if (event_group == NULL) {
    ESP_LOGE(TAG, "cannot create event group");
    return false;
  }

  player_start(STORAGE_SD_MOUNTPOINT   "/08 - IRQ 7 PRINTER.mp3");
  player_start(STORAGE_SD_MOUNTPOINT   "/01. The Time Machine (feat. Boys Noize).mp3");
  player_start(STORAGE_SD_MOUNTPOINT   "/03. Dreamside Dominions.mp3");
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
  while(true) {
    player_message_t message;
    if(xQueueReceive(queue, &message, portMAX_DELAY)) {
      switch(message.type) {
        case PLAYER_START:
          xEventGroupSetBits(event_group, PLAYING_BIT);
          play(message.filename);
          xEventGroupClearBits(event_group, PLAYING_BIT);
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
