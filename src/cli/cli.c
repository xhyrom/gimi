#include "../gimi_constants.h"
#include "command/ci.h"
#include "command/config.h"
#include "command/init.h"
#include "command/provider.h"
#include "command/push.h"
#include <stdio.h>
#include <string.h>

#define HELP                                                                   \
  "Usage: gimi [-h | --help] [-v | --version]\n\n"                             \
  "A simple tool for managing multiple git remotes as mirrors\n\n"             \
  "Useful commands for working with gimi:\n"                                   \
  "   ci         Generate CI configuration for a specific provider\n"          \
  "   config     Manage gimi configuration settings\n"                         \
  "   init       Initialize a gimi project in the current workspace\n"         \
  "   provider   Manage providers (git remotes as mirrors)\n"                  \
  "   push       Push all branches or tags to all providers\n\n"               \
  "Options:\n"                                                                 \
  "   -h, --help     Show this help message and exit\n"                        \
  "   -v, --version  Show the version of gimi"

void cli_print_help() { printf("%s", HELP); }
void cli_print_version() {
  printf("gimi version %d.%d.%d\n", GIMI_VERSION_MAJOR, GIMI_VERSION_MINOR,
         GIMI_VERSION_PATCH);
}

int cli_handle(int argc, char **argv) {
  char *sub_command = argv[0];

  if (strcmp(sub_command, "ci") == 0) {
    return cli_command_ci(argc, argv);
  }

  if (strcmp(sub_command, "config") == 0) {
    return cli_command_config(argc, argv);
  }

  if (strcmp(sub_command, "init") == 0) {
    return cli_command_init(argc, argv);
  }

  if (strcmp(sub_command, "provider") == 0) {
    return cli_command_provider(argc, argv);
  }

  if (strcmp(sub_command, "push") == 0) {
    return cli_command_push(argc, argv);
  }

  return 0;
}
