#include <stddef.h>
#include <stdio.h>
#include <sys/stat.h>

void generate_github_remote(char *buffer, size_t size,
                            const char *provider_name, const char *provider_ssh,
                            const char *repo_domain) {
  snprintf(buffer, size,
           "\n"
           "\n          git remote add gimi-%s %s"
           "\n          ssh-keyscan %s >> ~/.ssh/known_hosts"
           "\n          echo -e \"$SECRET\" >> ~/.ssh/gimi-%s"
           "\n          chmod 600 ~/.ssh/gimi-%s"
           "\n          echo -e \"Host %s\\nUser git\\nIdentityFile "
           "${HOME}/.ssh/gimi-%s\" >> ~/.ssh/config"
           "\n          git push -f --all gimi-%s"
           "\n          git push -f --tags gimi-%s",
           provider_name, provider_ssh, repo_domain, provider_name,
           provider_name, repo_domain, provider_name, provider_name,
           provider_name);
}

void generate_github(char *buffer, size_t size, const char *repo_domain,
                     const char *repo_owner, const char *repo_name,
                     const char *remotes) {
  snprintf(buffer, size,
           "name: gimi\n"
           "on: [push, delete]\n\n"
           "jobs:\n"
           "  sync:\n"
           "    runs-on: ubuntu-24.04\n"
           "    steps:\n"
           "      - uses: actions/checkout@v4\n"
           "        with:\n"
           "          fetch-depth: 0\n"
           "      - run: |\n"
           "          set +x"
           "          %s\n\n"
           "          set -x\n"
           "        env:\n"
           "          secret: ${{ secrets.SECRET }}",
           remotes);
}
