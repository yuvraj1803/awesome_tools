/*
    Author: Yuvraj Sakshith <ysakshith@gmail.com>
    Y: 2025
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>

#define BOLD_RED "\033[1;31m"
#define BOLD_GREEN "\033[1;32m"
#define GREEN "\033[32m"
#define RESET   "\033[0m"

int eg_is_git_repo(const char *dirname) {
    char gitpath[PATH_MAX];
    snprintf(gitpath, sizeof(gitpath), "%s/.git", dirname);
    return access(gitpath, F_OK) == 0;
}

void eg_print_git_info(struct dirent *entry) {

    printf(BOLD_RED "[GIT] %s/%s\n" RESET, getenv("PWD"), entry->d_name);

    char cmd[PATH_MAX];
    FILE *fp;
    const char *dirname = entry->d_name;
    int bsize = 128;
    int rsize = 128;
    int usize = 512;

    char *branch = (char*) malloc(bsize);
    char *remote = (char*) malloc(rsize);
    char *url = (char*) malloc(usize);

    snprintf(cmd, sizeof(cmd), "git -C '%s' rev-parse --abbrev-ref HEAD 2>/dev/null", dirname);
    fp = popen(cmd, "r");
    if (fp) {
        if (fgets(branch, bsize, fp)) {
            branch[strcspn(branch, "\n")] = 0;
        } else strcpy(branch, "(unknown)");
        pclose(fp);
    } else strcpy(branch, "(error)");

    snprintf(cmd, sizeof(cmd), "git -C '%s' remote 2>/dev/null", dirname);
    fp = popen(cmd, "r");
    if (fp) {
        if (fgets(remote, rsize, fp)) {
            remote[strcspn(remote, "\n")] = 0;
        } else strcpy(remote, "(none)");
        pclose(fp);
    } else strcpy(remote, "(error)");

    if (strcmp(remote, "(none)") != 0 && strcmp(remote, "(error)") != 0) {
        snprintf(cmd, sizeof(cmd), "git -C '%s' remote get-url %s 2>/dev/null", dirname, remote);
        fp = popen(cmd, "r");
        if (fp) {
            if (fgets(url, usize, fp))
                url[strcspn(url, "\n")] = 0;
            else strcpy(url, "(no url)");
            pclose(fp);
        } else strcpy(url, "(error)");
    } else {
        strcpy(url, "(no remote)");
    }

    printf(BOLD_GREEN "\t[BRANCH]: " GREEN "%s\n" RESET, branch);
    printf(BOLD_GREEN "\t[REMOTE]: " GREEN "%s\n" RESET, remote);
    printf(BOLD_GREEN "\t[URL]: " GREEN "%s\n" RESET, url);
    printf("\n");

}

int main() {

    DIR *dir;
    struct dirent *entry;
    struct stat st;

    dir = opendir(".");
    if (!dir) {
        perror("Unable to open current working directory.\n");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        if (stat(entry->d_name, &st) == 0 && S_ISDIR(st.st_mode)) {
            if (eg_is_git_repo(entry->d_name))
                eg_print_git_info(entry);
        }
    }

    closedir(dir);

	return 0;
}
