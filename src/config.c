#include "config.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <toml.h>

char *ssh_to_http(char *ssh) {
  if (ssh == NULL)
    return NULL;

  if (strncmp(ssh, "git@", 4) != 0)
    return NULL;

  size_t len = strlen(ssh);
  char *https = (char *)malloc(len + 10); // 8 for https://, 1 for /, 1 for \0
  if (https == NULL)
    return NULL;

  strcpy(https, "https://");

  char *colon = strchr(ssh + 4, ':');
  if (colon == NULL) {
    free(https);
    return NULL;
  }

  size_t domain_len = colon - (ssh + 4);
  strncat(https, ssh + 4, domain_len);

  strcat(https, "/");

  char *path = colon + 1;
  char *git_ext = strstr(path, ".git");
  if (git_ext != NULL) {
    *git_ext = '\0';
  }

  strcat(https, path);

  return https;
}

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

    provider->http = ssh_to_http(provider->ssh);

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

void config_write(struct gimi_config *cfg) {
  FILE *file_ptr;

  file_ptr = fopen(".gimi/config.toml", "w");
  if (!file_ptr) {
    return;
  }

  fprintf(file_ptr, "[providers]");

  for (int i = 0; i < cfg->providers_size; i++) {
    struct gimi_config_provider *provider = cfg->providers[i];

    fprintf(file_ptr, "\n[providers.%s]\nssh = \"%s\"", provider->name,
            provider->ssh);

    if (provider->primary) {
      fprintf(file_ptr, "\nprimary = true");
    }
  }

  fclose(file_ptr);
}

void config_free(struct gimi_config *cfg) {
  for (int i = 0; i < cfg->providers_size; i++) {
    free(cfg->providers[i]->name);
    free(cfg->providers[i]->ssh);
    free(cfg->providers[i]->http);
  }

  free(cfg->providers);
  free(cfg);
}

struct gimi_config_provider *config_find_provider(struct gimi_config *cfg,
                                                  char *name) {
  struct gimi_config_provider *provider = NULL;
  for (int i = 0; i < cfg->providers_size; i++) {
    if (strcmp(cfg->providers[i]->name, name) == 0) {
      provider = cfg->providers[i];
      break;
    }
  }

  return provider;
}
