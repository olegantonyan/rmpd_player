#ifndef __CLI_H
#define __CLI_H

#include "stm32f1xx_hal.h"

bool cli_init(UART_HandleTypeDef *uart);
void cli_received_callback();

#endif
