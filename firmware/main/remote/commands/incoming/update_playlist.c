#include "remote/commands/incoming.h"
#include <stdio.h>

bool update_playlist(const IncomingCommandArgument_t *arg) {

  printf("\n\n update_playlist %d\n\n", arg->sequence);

  return true;
}
