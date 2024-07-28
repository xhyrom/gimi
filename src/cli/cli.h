#define HANDLE_OPTIONS(argc, argv, optstring, ...)                             \
  int opt;                                                                     \
  opterr = 0;                                                                  \
  while ((opt = getopt(argc, argv, optstring)) != -1) {                        \
    switch (opt) {                                                             \
      __VA_ARGS__                                                              \
    default: {                                                                 \
      printf("unknown option -%c\n", (char)optopt);                            \
      break;                                                                   \
    }                                                                          \
    }                                                                          \
  }                                                                            \
  argc -= optind;                                                              \
  argv += optind;

#define OPTION(opt, func, ...)                                                 \
  case opt: {                                                                  \
    func(__VA_ARGS__);                                                         \
    break;                                                                     \
  }

#define MASK_OPTION(opt, mask, field)                                          \
  case opt: {                                                                  \
    mask |= field;                                                             \
    break;                                                                     \
  }

void cli_print_help();
void cli_print_version();

int cli_handle(int argc, char **argv);
