#pragma once

#include <stdbool.h>

#define ASSERT_CONFIG_EXIST(cfg)                                               \
  if (!cfg) {                                                                  \
    printf(                                                                    \
        "error: missing gimi config, initialize it using gimi init command."); \
    return 1;                                                                  \
  }

struct gimi_config_provider {
  char *name;
  char *ssh;
  bool primary;
};

struct gimi_config {
  int providers_size;
  struct gimi_config_provider **providers;
};

struct gimi_config *config_read();
void config_write(struct gimi_config *cfg);
void config_free(struct gimi_config *cfg);

struct gimi_config_provider *config_find_provider(struct gimi_config *cfg,
                                                  char *name);
