#include "../../config.h"
#include <stdio.h>

#define INIT_CONFIG "[providers]\n"

int cli_command_config(int argc, char **argv) {
  struct gimi_config *cfg = config_read();
  ASSERT_CONFIG_EXIST(cfg);

  for (int i = 0; i < cfg->providers_size; i++) {
    struct gimi_config_provider *provider = cfg->providers[i];
    printf("name: %s | ssh: %s | primary: %d\n", provider->name, provider->ssh,
           provider->primary);
  }

  config_free(cfg);
  return 0;
}
