#include "cli/cli.h"
#include <string.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    cli_print_help();
    return 1;
  }

  if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    cli_print_help();
    return 0;
  }

  if (strcmp(argv[1], "-v") == 0 || strcmp(argv[1], "--version") == 0) {
    cli_print_version();
    return 0;
  }

  argc -= 1;
  argv += 1;

  cli_handle(argc, argv);

  return 0;
}
