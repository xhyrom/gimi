#include "../../config.h"
#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define SOURCEHUT                                                              \
  "image: alpine/latest\n"                                                     \
  "packages:\n"                                                                \
  "  - git\n"                                                                  \
  "sources:\n"                                                                 \
  "  - \"https://%s/%s/%s\"\n"                                                 \
  "secrets:\n"                                                                 \
  "  - <add>\n"                                                                \
  "tasks:\n"                                                                   \
  "  - sync: |\n"                                                              \
  "      cd %s\n"                                                              \
  "      set +x"                                                               \
  "      %s\n\n"                                                               \
  "      set -x\n"

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

char *generate_sourcehut(struct gimi_config *cfg,
                         struct gimi_config_provider *main_provider) {
  char remotes[1024] = "";

  for (int i = 0; i < cfg->providers_size; i++) {
    struct gimi_config_provider *provider = cfg->providers[i];
    if (strcmp(provider->name, main_provider->name) == 0)
      continue;

    struct repository *repo = translate_ssh_to_repository(provider->ssh);

    char buf[256];
    snprintf(buf, sizeof(buf),
             "\n"
             "\n      git remote add gimi-%s %s "
             "\n      ssh-keyscan %s >> ~/.ssh/known_hosts"
             "\n      git push -f --all gimi-%s"
             "\n      git push -f --tags gimi-%s",
             provider->name, provider->ssh, repo->domain, provider->name,
             provider->name);

    strcat(remotes, buf);

    repository_free(repo);
  }

  char *buf = (char *)malloc(2048);
  struct repository *repo = translate_ssh_to_repository(main_provider->ssh);

  snprintf(buf, 2048, SOURCEHUT, repo->domain, repo->owner, repo->repo,
           repo->repo, remotes);

  repository_free(repo);

  return buf;
}

int generate(int argc, char **argv) {
  struct gimi_config *cfg = config_read();
  struct gimi_config_provider *provider;

  if (argc == 2) {
    provider = config_find_provider(cfg, argv[1]);

    if (provider == NULL) {
      printf("error: no such provider '%s'", argv[1]);
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
      printf("error: can't find primary provider");
      return 1;
    }
  };

  char *template;
  if (strcmp(provider->name, "sourcehut") == 0) {
    template = generate_sourcehut(cfg, provider);

    int ret = mkdir(".builds", S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
    if (ret != 0 && errno != EEXIST) {
      printf("error: failed to create directory .builds with errno %d", errno);
      return ret;
    }

    FILE *file_ptr;
    file_ptr = fopen(".builds/gimi.yml", "w");
    if (file_ptr == NULL) {
      printf("error: failed to open file .builds/gimi.yml");
      return 1;
    }

    fprintf(file_ptr, "%s", template);

    fclose(file_ptr);
  } else {
    printf("error: provider '%s' is not supported, create the ci yourself.",
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
    printf("usage: gimi ci generate [provider]");
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
