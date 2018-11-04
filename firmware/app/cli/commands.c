#include "cli/commands.h"
#include "os.h"
#include "FreeRTOS_CLI.h"
#include "clock/clock.h"
#include <stdio.h>

static BaseType_t x_date(char *buffer, size_t length, const char *cmd) {
  time_t current_time = time(NULL);
  snprintf(buffer, length, "%s", ctime(&current_time));
  return pdFALSE;
}

static const CLI_Command_Definition_t x_date_command = {
  "date",
  "date: returns current date/time\r\n",
  x_date,
  0
};

void cli_commands_init() {
  FreeRTOS_CLIRegisterCommand(&x_date_command);
}
