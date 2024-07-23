#include "../../config.h"
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int providers() {
  struct gimi_config *cfg = config_read();
  ASSERT_CONFIG_EXIST(cfg);

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
  ASSERT_CONFIG_EXIST(cfg);

  struct gimi_config_provider *provider = config_find_provider(cfg, argv[1]);

  if (!provider) {
    printf("error: no such provider '%s'", argv[1]);
    return 1;
  }

  printf("name: %s\n", provider->name);
  printf("ssh: %s\n", provider->ssh);
  printf("primary: %d\n", provider->primary);

  config_free(cfg);
  free(provider);

  return 0;
}

int provider_sync(int argc, char **argv) {
  if (argc == 1) {
    printf("usage: gimi provider sync [--all] [name]");
    return 1;
  }

  struct gimi_config *cfg = config_read();
  ASSERT_CONFIG_EXIST(cfg);

  struct gimi_config_provider *provider = config_find_provider(cfg, argv[1]);

  char command[100];
  snprintf(command, sizeof(command), "git remote add gimi-%s %s",
           provider->name, provider->ssh);

  int ret = system(command);
  if (ret != 0) {
    printf("error: failed to sync provider '%s' with git's exit code %d.",
           provider->name, ret);
  } else {
    printf("info: provider '%s' has been successfully synced with git.",
           provider->name);
  }

  config_free(cfg);
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

  if (strcmp(subcommand, "sync") == 0) {
    return provider_sync(argc, argv);
  }

  return 0;
}
