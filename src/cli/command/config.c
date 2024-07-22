#include "../../config.h"
#include "../cli.h"
#include <stdio.h>

#define INIT_CONFIG "[providers]\n"

void cli_command_config(int argc, char **argv) {
  struct gimi_config *cfg = config_read();
  if (!cfg) {
    printf("Missing gimi config. Initialize it using gimi init command.");
    return;
  }

  for (int i = 0; i < cfg->providers_size; i++) {
    struct gimi_config_provider *provider = cfg->providers[i];
    printf("ssh: %s | primary: %d\n", provider->ssh, provider->primary);
  }

  config_free(cfg);
}
