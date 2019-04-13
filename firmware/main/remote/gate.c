#include "remote/gate.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi/wifi.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "remote/http.h"
#include "remote/queue.h"
#include "remote/commands/incoming.h"
#include "system/status.h"

static const char *TAG = "remote_gate";

static void thread(void *_args);

bool gate_init() {
  BaseType_t task_created = xTaskCreate(thread, TAG, 5200 + REMOTE_HTTP_MAX_RECEIVE_DATA_LENGTH, NULL, 5, NULL);
  if (pdPASS != task_created) {
    ESP_LOGE(TAG, "cannot create thread");
    return false;
  }
  return true;
}

static void thread(void *_args) {
  wifi_wait_connected(portMAX_DELAY);
  vTaskDelay(pdMS_TO_TICKS(500)); // for some reasons, probably

  http_response_t recv;
  QueueMessage_t msg;

  while (true) {
    memset(&msg, 0, sizeof(msg));

    if (queue_receive(&msg, portMAX_DELAY)) {
      if (msg.data != NULL) {

        do {
          memset(&recv, 0, sizeof(recv));
          int status = http_post_cmd(msg.data, strlen(msg.data), msg.sequence, &recv);
          if (status >= 200 && status < 300) {
            if (!incoming_command(recv.data, recv.datafile, recv.sequence)) {
              ESP_LOGD(TAG, "error executing incoming command");
            }
            status_set_online(true);
            if (msg.callback != NULL) {
              msg.callback(true);
            }
            break;
          } else {
            status_set_online(false);
            if (msg.callback != NULL) {
              msg.callback(false);
            }
          }
          vTaskDelay(pdMS_TO_TICKS(5678));
        } while(msg.max_retries-- > 0);

        free(msg.data);
        msg.data = NULL;
      }
    }
    taskYIELD();
  }
}
