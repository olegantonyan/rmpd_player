#include "cJSON.h"
#include "remote/commands/incoming.h"
#include <stdio.h>

bool update_setting(const IncomingCommandArgument_t *arg) {

  printf("\n\nupdate_setting %d\n\n", arg->sequence);

  return true;
}
