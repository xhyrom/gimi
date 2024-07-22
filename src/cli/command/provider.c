#include "../../config.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int providers() {
  struct gimi_config *cfg = config_read();
  if (!cfg)
    return 1;

  for (int i = 0; i < cfg->providers_size; i++) {
    struct gimi_config_provider *provider = cfg->providers[i];
    printf("%s\n", provider->name);
  }

  config_free(cfg);

  return 0;
}

int provider_info(int argc, char **argv) {
  if (argc == 1) {
    printf("usage: gimi provider info <name>");
    return 1;
  }

  struct gimi_config *cfg = config_read();
  if (!cfg)
    return 1;

  struct gimi_config_provider *provider = NULL;
  for (int i = 0; i < cfg->providers_size; i++) {
    if (strcmp(cfg->providers[i]->name, argv[1]) == 0) {
      provider = cfg->providers[i];
    }
  }

  config_free(cfg);

  if (!provider) {
    printf("No such provider '%s'", argv[1]);
    return 1;
  }

  printf("name: %s\n", provider->name);
  printf("ssh: %s\n", provider->ssh);
  printf("primary: %d\n", provider->primary);

  free(provider);

  return 0;
}

int cli_command_provider(int argc, char **argv) {
  if (argc == 1) {
    return providers();
  }

  // remove "provider" from args
  argc -= 1;
  argv += 1;

  char *subcommand = argv[0];

  if (strcmp(subcommand, "info") == 0) {
    return provider_info(argc, argv);
  }

  return 0;
}
