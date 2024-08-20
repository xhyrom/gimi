#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>

void generate_sourcehut_remote(char *buffer, size_t size,
                               const char *provider_name,
                               const char *provider_ssh,
                               const char *repo_domain) {
  snprintf(buffer, size,
           "\n"
           "\n      git remote add gimi-%s %s"
           "\n      ssh-keyscan %s >> ~/.ssh/known_hosts"
           "\n      echo -e \"Host %s\\nUser git\\nIdentityFile "
           "${HOME}/.ssh/<secret>\" >> ~/.ssh/config"
           "\n      git push -f --all gimi-%s"
           "\n      git push -f --tags gimi-%s",
           provider_name, provider_ssh, repo_domain, repo_domain, provider_name,
           provider_name);
}

void generate_sourcehut(char *buffer, size_t size, const char *repo_domain,
                        const char *repo_owner, const char *repo_name,
                        const char *remotes) {
  snprintf(buffer, size,
           "image: alpine/latest\n"
           "packages:\n"
           "  - git\n"
           "sources:\n"
           "  - \"https://%s/%s/%s\"\n"
           "secrets:\n"
           "  - <secret>\n"
           "tasks:\n"
           "  - sync: |\n"
           "      cd %s\n"
           "      set +x"
           "      %s\n\n"
           "      set -x\n",
           repo_domain, repo_owner, repo_name, repo_name, remotes);
}
