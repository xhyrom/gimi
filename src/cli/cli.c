#include "../gimi_constants.h"
#include "command/config.h"
#include "command/init.h"
#include <stdio.h>
#include <string.h>

#define HELP                                                                   \
  "Usage: gimi [-h | --help] [-v | --version]\n\n"                             \
  "A simple tool for managing multiple git remotes as mirrors\n\n"             \
  "Commands\n"

void cli_print_help() { printf("%s", HELP); }
void cli_print_version() {
  printf("gimi version %d.%d.%d\n", GIMI_VERSION_MAJOR, GIMI_VERSION_MINOR,
         GIMI_VERSION_PATCH);
}

int cli_handle(int argc, char **argv) {
  char *sub_command = argv[0];

  if (strcmp(sub_command, "init") == 0) {
    return cli_command_init(argc, argv);
  }

  if (strcmp(sub_command, "config") == 0) {
    return cli_command_config(argc, argv);
  }

  return 0;
}
