#ifndef __CLI_H
#define __CLI_H

#include "stm32f1xx_hal.h"

#define CLI_MAX_INPUT_LENGTH (72)

bool cli_init(UART_HandleTypeDef *uart);
void cli_callback();

#endif
