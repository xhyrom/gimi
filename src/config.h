#include <stdbool.h>

typedef struct {
  char *ssh;
  bool primary;
} gimi_config_provider;

typedef struct {
  int providers_size;
  gimi_config_provider **providers;
} gimi_config;

gimi_config *config_read();
