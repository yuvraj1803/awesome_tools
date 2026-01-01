#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF 32768

int main(int argc, char **argv) {
    if (argc < 3 || argc > 4) {
        fprintf(stderr,
            "Usage: %s <commit> \"subject\" [\"body\"]\n", argv[0]);
        return 1;
    }

    const char *commit = argv[1];
    const char *subject = argv[2];
    const char *body_override = (argc == 4) ? argv[3] : NULL;

    char cmd[BUF], oldmsg[BUF] = {0};

    /* Read existing commit message */
    snprintf(cmd, sizeof(cmd),
             "git show -s --format=%%B %s", commit);
    FILE *fp = popen(cmd, "r");
    if (!fp) return 1;
    fread(oldmsg, 1, sizeof(oldmsg) - 1, fp);
    pclose(fp);

    /* Extract existing body */
    char *old_body = strchr(oldmsg, '\n');
    if (old_body) old_body++;
    else old_body = "";

    /* Write new message */
    FILE *out = fopen(".git-newmsg", "w");
    if (!out) return 1;

    fprintf(out, "%s\n", subject);
    if (body_override)
        fprintf(out, "\n%s\n", body_override);
    else
        fprintf(out, "%s", old_body);

    fclose(out);

    /* Detect root commit */
    snprintf(cmd, sizeof(cmd),
             "git rev-list --parents -n 1 %s | wc -w", commit);
    fp = popen(cmd, "r");
    int words = 0;
    fscanf(fp, "%d", &words);
    pclose(fp);

    /* Prepare rebase */
    if (words == 1)
        snprintf(cmd, sizeof(cmd),
            "GIT_EDITOR='cp .git-newmsg' git rebase -i --root");
    else
        snprintf(cmd, sizeof(cmd),
            "GIT_EDITOR='cp .git-newmsg' git rebase -i %s^", commit);

    setenv("GIT_SEQUENCE_EDITOR",
           "sed -i '' 's/^pick/reword/'", 1);

    int rc = system(cmd);
    remove(".git-newmsg");
    return rc;
}

