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

static const char *TAG = "remote_gate";

static void thread(void *_args);

bool gate_init() {
  BaseType_t task_created = xTaskCreate(thread, "remote_gate", 5000 + REMOTE_HTTP_MAX_RECEIVE_DATA_LENGTH, NULL, 5, NULL);
  if (pdPASS != task_created) {
    ESP_LOGE(TAG, "cannot create thread");
    return false;
  }
  return true;
}

static void thread(void *_args) {
  wifi_wait_connected(portMAX_DELAY);
  vTaskDelay(pdMS_TO_TICKS(1234)); // for some reasons, probably

  http_response_t recv;
  QueueMessage_t msg;

  while (true) {
    memset(&msg, 0, sizeof(msg));

    if (queue_receive(&msg, portMAX_DELAY)) {
      if (msg.data != NULL) {

        while(true) {
          memset(&recv, 0, sizeof(recv));
          printf("\n\n%s\n\n", (char *)msg.data);
          int status = http_post_cmd(msg.data, strlen(msg.data), 0, &recv);
          if (status >= 200 && status < 300) {
            printf("data: %s\n", recv.data);
            printf("stat: %d\n", status);
            printf("seq:  %d\n", recv.sequence);
            printf("len:  %d\n", recv.length);
            break;
          }
          vTaskDelay(pdMS_TO_TICKS(5678));
        }


        free(msg.data);
        msg.data = NULL;
      }
    }
    taskYIELD();
  }
}
