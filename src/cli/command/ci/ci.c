#include "../../../config.h"
#include "github.h"
#include "sourcehut.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define CREATE_DIR(path)                                                       \
  int ret = mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);      \
  if (ret != 0 && errno != EEXIST) {                                           \
    printf("error: failed to create directory %s with errno %d\n", path,       \
           errno);                                                             \
    return ret;                                                                \
  }

#define CREATE_DIRS(...)                                                       \
  const char *dirs[] = {__VA_ARGS__};                                          \
  for (size_t i = 0; i < sizeof(dirs) / sizeof(dirs[0]); ++i) {                \
    CREATE_DIR(dirs[i]);                                                       \
  }

#define WRITE_TEMPLATE_TO_FILE(path, template)                                 \
  FILE *file_ptr = fopen(path, "w");                                           \
  if (file_ptr == NULL) {                                                      \
    printf("error: failed to open file %s\n", path);                           \
    return 1;                                                                  \
  }                                                                            \
  fprintf(file_ptr, "%s", template);                                           \
  fclose(file_ptr);

#define HANDLE_PROVIDER(provider_name, file_path, remote_func, gen_func, ...)  \
  if (strcmp(provider->name, provider_name) == 0) {                            \
    char *template =                                                           \
        generate_file_content(cfg, provider, remote_func, gen_func);           \
    CREATE_DIRS(__VA_ARGS__);                                                  \
    WRITE_TEMPLATE_TO_FILE(file_path, template);                               \
  }

struct repository {
  char *domain;
  char *owner;
  char *repo;
};

struct repository *translate_ssh_to_repository(char *ssh) {
  const char *at = strchr(ssh, '@');
  const char *colon = strchr(ssh, ':');
  const char *slash = strchr(colon, '/');

  if (!at || !colon || at > colon) {
    return NULL;
  }

  struct repository *repo =
      (struct repository *)malloc(sizeof(struct repository));
  if (!repo) {
    return NULL;
  }

  int domain_len = colon - at - 1;
  repo->domain = (char *)malloc(domain_len + 1);
  snprintf(repo->domain, domain_len + 1, "%.*s", domain_len, at + 1);

  int repo_len = slash - colon - 1;
  repo->owner = (char *)malloc(repo_len + 1);
  snprintf(repo->owner, repo_len + 1, "%.*s", repo_len, colon + 1);

  int slash_len = strlen(slash) - 1;
  repo->repo = (char *)malloc(slash_len + 1);
  snprintf(repo->repo, slash_len + 1, "%s", slash + 1);

  return repo;
}

void repository_free(struct repository *repo) {
  free(repo->owner);
  free(repo->repo);
  free(repo);
}

char *generate_file_content(
    struct gimi_config *cfg, struct gimi_config_provider *main_provider,
    void (*generate_remote)(char *, size_t, const char *, const char *,
                            const char *),
    void (*generate_base)(char *, size_t, const char *, const char *,
                          const char *, const char *)) {
  char remotes[1024] = "";

  for (int i = 0; i < cfg->providers_size; i++) {
    struct gimi_config_provider *provider = cfg->providers[i];
    if (strcmp(provider->name, main_provider->name) == 0)
      continue;

    struct repository *repo = translate_ssh_to_repository(provider->ssh);

    char buf[1024];

    generate_remote(buf, sizeof(buf), provider->name, provider->ssh,
                    repo->domain);

    strcat(remotes, buf);

    repository_free(repo);
  }

  char *buf = (char *)malloc(2048);
  struct repository *repo = translate_ssh_to_repository(main_provider->ssh);

  generate_base(buf, 2048, repo->domain, repo->owner, repo->repo, remotes);

  repository_free(repo);

  return buf;
}

int create_directory(const char *path) {
  int ret = mkdir(path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
  if (ret != 0 && errno != EEXIST) {
    printf("error: failed to create directory %s with errno %d\n", path, errno);
    return ret;
  }
  return 0;
}

int write_template_to_file(const char *path, const char *template) {
  FILE *file_ptr = fopen(path, "w");
  if (file_ptr == NULL) {
    printf("error: failed to open file %s\n", path);
    return 1;
  }
  fprintf(file_ptr, "%s", template);
  fclose(file_ptr);
  return 0;
}

int generate(int argc, char **argv) {
  struct gimi_config *cfg = config_read();
  ASSERT_CONFIG_EXIST(cfg);

  struct gimi_config_provider *provider = NULL;

  if (argc == 2) {
    provider = config_find_provider(cfg, argv[1]);
    if (provider == NULL) {
      printf("error: no such provider '%s'\n", argv[1]);
      return 1;
    }
  } else {
    for (int i = 0; i < cfg->providers_size; i++) {
      if (cfg->providers[i]->primary) {
        provider = cfg->providers[i];
        break;
      }
    }
    if (provider == NULL) {
      printf("error: can't find primary provider\n");
      return 1;
    }
  }

  HANDLE_PROVIDER("sourcehut", ".builds/gimi.yml", generate_sourcehut_remote,
                  generate_sourcehut, ".builds");
  HANDLE_PROVIDER("github", ".github/workflows/gimi.yml",
                  generate_github_remote, generate_github, ".github",
                  ".github/workflows");

  if (provider == NULL) {
    printf("error: provider '%s' is not supported, create the ci yourself.\n",
           provider->name);
    config_free(cfg);
    free(provider);
    return 1;
  }

  config_free(cfg);
  free(provider);
  return 0;
}

int cli_command_ci(int argc, char **argv) {
  if (argc == 1) {
    printf("usage: gimi ci generate [provider]\n");
    return 1;
  }

  // remove "ci" from args
  argc -= 1;
  argv += 1;

  char *subcommand = argv[0];

  if (strcmp(subcommand, "generate") == 0) {
    return generate(argc, argv);
  }

  return 0;
}
