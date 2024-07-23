#include "../../config.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#define SOURCEHUT                                                              \
  "image: alpine/latest\n"                                                     \
  "packages:\n"                                                                \
  "  - git\n"                                                                  \
  "sources:\n"                                                                 \
  "  - %s\n"                                                                   \
  "secrets:\n"                                                                 \
  "  - <add>\n"                                                                \
  "tasks:\n"                                                                   \
  "  - sync: |\n"                                                              \
  "      %s\n"                                                                 \
  "      %s\n"                                                                 \
  "      %s\n"

int generate(int argc, char **argv) {
  struct gimi_config *cfg = config_read();
  struct gimi_config_provider *provider;

  if (argc == 2) {
    provider = config_find_provider(cfg, argv[1]);

    if (provider == NULL) {
      printf("error: no such provider '%s'", argv[1]);
      return 1;
    }
  } else {
    for (int i = 0; i < cfg->providers_size; i++) {
      if (cfg->providers[i]->primary) {
        provider = cfg->providers[i];
        break;
      }
    }

    if (provider == NULL) {
      printf("error: can't find primary provider");
      return 1;
    }
  };

  config_free(cfg);
  return 0;
}

int cli_command_ci(int argc, char **argv) {
  if (argc == 1) {
    printf("usage: gimi ci generate [provider]");
    return 1;
  }

  // remove "ci" from args
  argc -= 1;
  argv += 1;

  char *subcommand = argv[0];

  if (strcmp(subcommand, "generate") == 0) {
    return generate(argc, argv);
  }

  return 0;
}
