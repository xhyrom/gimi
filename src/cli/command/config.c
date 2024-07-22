#include "../../config.h"
#include "../cli.h"
#include <stdio.h>

#define INIT_CONFIG "[providers]\n"

int cli_command_config(int argc, char **argv) {
  struct gimi_config *cfg = config_read();
  if (!cfg) {
    printf(
        "error: missing gimi config, initialize it using gimi init command.");
    return 1;
  }

  for (int i = 0; i < cfg->providers_size; i++) {
    struct gimi_config_provider *provider = cfg->providers[i];
    printf("name: %s | ssh: %s | primary: %d\n", provider->name, provider->ssh,
           provider->primary);
  }

  config_free(cfg);
  return 0;
}
