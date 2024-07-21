#include "cli/cli.h"
#include <string.h>

int main(int argc, char **argv) {
  if (argc == 1) {
    cli_print_help();
    return 0;
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

  /*int opt;
  opterr = 0;
  while ((opt = getopt(argc, argv, "hv")) != -1) {
    switch (opt) {
    case 'h': {
      print_help();
      break;
    }
    case 'v': {
      print_version();
      break;
    }
    default: {
      printf("unknown option -%c\n", (char)optopt);
      return 1;
    }
    }
  }
  argc -= optind;
  argv += optind;
*/
  return 0;
}
