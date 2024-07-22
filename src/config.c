#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toml.h>

struct gimi_config *config_read() {
  FILE *file_ptr;
  char errbuf[200];

  file_ptr = fopen(".gimi/config.toml", "r");
  if (!file_ptr) {
    return NULL;
  }

  toml_table_t *toml_cfg = toml_parse_file(file_ptr, errbuf, sizeof(errbuf));
  fclose(file_ptr);

  toml_table_t *toml_providers = toml_table_in(toml_cfg, "providers");

  int size = 0;
  const char *key;
  while ((key = toml_key_in(toml_providers, size))) {
    size++;
  }

  struct gimi_config *cfg =
      (struct gimi_config *)malloc(sizeof(struct gimi_config));

  cfg->providers_size = size;
  cfg->providers =
      malloc(cfg->providers_size * sizeof(struct gimi_config_provider));

  for (int i = 0; i < size; i++) {
    const char *key = toml_key_in(toml_providers, i);

    toml_table_t *toml_provider = toml_table_in(toml_providers, key);

    struct gimi_config_provider *provider =
        (struct gimi_config_provider *)malloc(
            sizeof(struct gimi_config_provider));

    provider->name = strdup(key);

    toml_datum_t ssh = toml_string_in(toml_provider, "ssh");
    provider->ssh = strdup(ssh.u.s);

    toml_datum_t toml_primary = toml_bool_in(toml_provider, "primary");
    if (!toml_primary.ok) {
      provider->primary = 0;
    } else {
      provider->primary = toml_primary.u.b;
    }

    cfg->providers[i] = provider;

    free(ssh.u.s);
  }

  toml_free(toml_cfg);

  return cfg;
}

void config_free(struct gimi_config *cfg) {
  free(cfg->providers);
  free(cfg);
}

struct gimi_config_provider *config_find_provider(struct gimi_config *cfg,
                                                  char *name) {
  struct gimi_config_provider *provider = NULL;
  for (int i = 0; i < cfg->providers_size; i++) {
    if (strcmp(cfg->providers[i]->name, name) == 0) {
      provider = cfg->providers[i];
    }
  }

  return provider;
}
