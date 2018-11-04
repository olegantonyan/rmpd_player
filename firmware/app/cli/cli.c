#include "cli/cli.h"
#include "os.h"
#include "FreeRTOS_CLI.h"
#include "cli/commands.h"

#include <string.h>

static void thread(void *params);
static void uart_receive();
static void clear_buffer();

static UART_HandleTypeDef *uart = NULL;
static MessageBufferHandle_t channel = NULL;
static struct {
  uint8_t data[CLI_MAX_INPUT_LENGTH];
  size_t index;
} rx_buffer = { {0}, 0 };

bool cli_init(UART_HandleTypeDef *uart_) {
  uart = uart_;

  channel = xMessageBufferCreate(sizeof(rx_buffer.data));
  uart_receive();
  cli_commands_init();

  TaskHandle_t handle = NULL;
  BaseType_t result = xTaskCreate(thread, "cli_thread", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &handle);
  return result == pdPASS;
}

void cli_callback() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  if (rx_buffer.data[rx_buffer.index] == '\n') {
    xMessageBufferSendFromISR(channel, rx_buffer.data, strlen((const char *)rx_buffer.data), &xHigherPriorityTaskWoken);
    clear_buffer();
  } else if (++rx_buffer.index >= sizeof(rx_buffer.data)) {
    clear_buffer();
  }

  uart_receive();
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// private

static void thread(void *params) {
  while(true) {
    static char buf[sizeof(rx_buffer.data)] = { 0 };
    static char out[configCOMMAND_INT_MAX_OUTPUT_SIZE] = { 0 };
    if(xMessageBufferReceive(channel, buf, sizeof(buf), portMAX_DELAY) > 0) {
      for(size_t i = 0; i < strlen(buf); i++) {
        if (buf[i] == '\n' || buf[i] == '\r') {
          buf[i] = '\0';
        } 
      }
      BaseType_t res = pdFALSE;
      do {
        res = FreeRTOS_CLIProcessCommand(buf, out, sizeof(out));
        printf(out);
      } while (res != pdFALSE);
    }
  }
}

static void uart_receive() {
  if (!uart) {
    return;
  }
  HAL_UART_Receive_IT(uart, &rx_buffer.data[rx_buffer.index], 1);
}

static void clear_buffer() {
  memset(&rx_buffer, 0, sizeof(rx_buffer));
}
