#include "../../config.h"
#include "../cli.h"
#include <linux/limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *get_current_branch_name() {
  FILE *file_ptr;
  char output[256];

  file_ptr = popen("git rev-parse --abbrev-ref HEAD", "r");
  if (file_ptr == NULL) {
    return NULL;
  }

  char *branch = fgets(output, sizeof(output), file_ptr);
  if (branch != NULL) {
    // remove new line
    size_t len = strlen(branch);
    if (len > 0 && branch[len - 1] == '\n') {
      branch[len - 1] = '\0';
    }
  }

  pclose(file_ptr);

  return branch;
}

int git_push(char *provider_name, char *branch_name, bool tags, bool verbose) {
  FILE *file_ptr;
  char output[1024];
  char command[256];

  char push_args[50] = "";
  if (tags) {
    strcat(push_args, " --tags");
  } else {
    if (strcmp(provider_name, "sourcehut") == 0) {
      strcat(push_args, " -o skip-ci");
    } else if (strcmp(provider_name, "gitlab") == 0) {
      strcat(push_args, " -o ci.skip");
    }
  }

  snprintf(command, sizeof(command), "git push%s gimi-%s %s 2>&1", push_args,
           provider_name, branch_name);

  file_ptr = popen(command, "r");
  if (file_ptr == NULL) {
    return 0;
  }

  while (fgets(output, sizeof(output), file_ptr) !=
         NULL) { // need to process for valid exit code
    if (verbose) {
      printf("%s", output);
    }
  }

  int ret = WEXITSTATUS(pclose(file_ptr));
  return ret;
}

void set_verbose(bool *verbose) { *verbose = true; }
void set_tags(bool *tags) { *tags = true; }

int cli_command_push(int argc, char **argv) {
  struct gimi_config *cfg = config_read();
  ASSERT_CONFIG_EXIST(cfg);

  bool tags = false;
  bool verbose = false;

  HANDLE_OPTIONS(argc, argv, "tv",
                 OPTION_WITH_ARG('t', set_tags, &tags)
                     OPTION_WITH_ARG('v', set_verbose, &verbose));

  char *branch_name = get_current_branch_name();

  for (int i = 0; i < cfg->providers_size; i++) {
    struct gimi_config_provider *provider = cfg->providers[i];

    int ret = git_push(provider->name, branch_name, tags, verbose);
    if (ret != 0) {
      printf("error: failed to push into '%s' with git's exit code %d.\n",
             provider->name, ret);
      break;
    }

    printf("info: successfully pushed into '%s'.\n", provider->name);
  }

  config_free(cfg);

  return 0;
}
