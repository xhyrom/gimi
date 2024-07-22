#include <stdbool.h>

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
void config_free(struct gimi_config *cfg);
