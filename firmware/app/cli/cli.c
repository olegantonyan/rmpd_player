#include "cli/cli.h"
#include "os.h"
#include "FreeRTOS_CLI.h"
#include "cli/commands.h"

#include <string.h>

static void thread(void *params);
static void uart_receive();
static bool uart_transmit(const char *str);
static void clear_buffer();

static UART_HandleTypeDef *uart = NULL;
static MessageBufferHandle_t channel = NULL;
static struct {
  uint8_t data[configCOMMAND_INT_MAX_INPUT_SIZE];
  size_t index;
} rx_buffer = { {0}, 0 };

bool cli_init(UART_HandleTypeDef *uart_) {
  uart = uart_;

  channel = xMessageBufferCreate(sizeof(rx_buffer.data));
  uart_receive();
  cli_commands_init();

  TaskHandle_t handle = NULL;
  BaseType_t result = xTaskCreate(thread, "cli", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, &handle);
  return result == pdPASS;
}

void cli_received_callback() {
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
    //static char input[configCOMMAND_INT_MAX_INPUT_SIZE] = { 0 };
    //static char output[configCOMMAND_INT_MAX_OUTPUT_SIZE] = { 0 };
    char *input = pvPortMalloc(configCOMMAND_INT_MAX_INPUT_SIZE);
    char *output = pvPortMalloc(configCOMMAND_INT_MAX_OUTPUT_SIZE);
    if(xMessageBufferReceive(channel, input, configCOMMAND_INT_MAX_INPUT_SIZE, portMAX_DELAY) > 0) {
      for(size_t i = 0; i < strlen(input); i++) {
        if (input[i] == '\n' || input[i] == '\r') {
          input[i] = '\0';
        }
      }
      BaseType_t res = pdFALSE;
      do {
        res = FreeRTOS_CLIProcessCommand(input, output, configCOMMAND_INT_MAX_OUTPUT_SIZE);
        uart_transmit(output);
      } while (res != pdFALSE);
    }
    //vPortFree(input);
    //vPortFree(output);
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

static bool uart_transmit(const char *str) {
  if (!uart) {
    return false;
  }
  return HAL_UART_Transmit(uart, (uint8_t *)str, strlen(str), 100) == HAL_OK;
}
