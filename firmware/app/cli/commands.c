#include "cli/commands.h"
#include "os.h"
#include "FreeRTOS_CLI.h"
#include "clock/clock.h"
#include <string.h>
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


static BaseType_t x_ps(char *buffer, size_t length, const char *cmd) {
  const char *const header = "Task          State  Priority  Stack	#\r\n************************************************\r\n";
	strcpy(buffer, header);
	vTaskList(buffer + strlen(header));
  return pdFALSE;
}
static const CLI_Command_Definition_t x_ps_command = {
  "ps",
  "ps: print task list\r\n",
  x_ps,
  0
};


static BaseType_t x_psa(char *buffer, size_t length, const char *cmd) {
  const char *const header = "Task          Abs time             % Time\r\n************************************************\r\n";
	strcpy(buffer, header);
	vTaskGetRunTimeStats(buffer + strlen(header));
  return pdFALSE;
}
static const CLI_Command_Definition_t x_psa_command = {
  "psa",
  "psa: print runtime stats\r\n",
  x_psa,
  0
};

void cli_commands_init() {
  FreeRTOS_CLIRegisterCommand(&x_date_command);
  FreeRTOS_CLIRegisterCommand(&x_ps_command);
  FreeRTOS_CLIRegisterCommand(&x_psa_command);
}
