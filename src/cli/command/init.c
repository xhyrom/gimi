#include "../cli.h"
#include <errno.h>
#include <linux/limits.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

#define INIT_CONFIG "[providers]\n"

int cli_command_init(int argc, char **argv) {
  errno = 0;
  int ret = mkdir(".gimi", S_IRWXU);
  if (ret == -1 && errno != EEXIST) {
    printf("Failed to initialize gimi.\n");
    return 1;
  }

  struct stat stats;
  if (stat(".git", &stats) != 0 || !S_ISDIR(stats.st_mode)) {
    printf("Missing .git directory. Use git init to initialize git.\n");
    return 1;
  }

  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) == NULL) {
    printf("Failed to get current working directory.\n");
    return 1;
  }

  printf("Initialized gimi in %s/.gimi\n", cwd);

  FILE *file_ptr;
  file_ptr = fopen(".gimi/config.toml", "w");

  fprintf(file_ptr, INIT_CONFIG);
  fclose(file_ptr);

  return 0;
}
