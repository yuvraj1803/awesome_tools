#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF 65536

static void die(const char *msg) {
    fprintf(stderr, "error: %s\n", msg);
    exit(1);
}

static void run(const char *cmd) {
    if (system(cmd) != 0)
        die(cmd);
}

int main(int argc, char **argv) {
    if (argc < 3 || argc > 4)
        die("usage: easygit_modify_commit <commit> \"subject\" [\"body\"]");

    const char *commit = argv[1];
    const char *subject = argv[2];
    const char *body_override = (argc == 4) ? argv[3] : NULL;

    char cmd[BUF], msg[BUF] = {0};

    /* ensure commit exists */
    snprintf(cmd, sizeof(cmd), "git cat-file -e %s^{commit}", commit);
    run(cmd);

    /* ensure commit is ancestor of HEAD */
    snprintf(cmd, sizeof(cmd), "git merge-base --is-ancestor %s HEAD", commit);
    if (system(cmd) != 0)
        die("commit is not ancestor of HEAD");

    /* refuse if commit is reachable from another branch */
    snprintf(cmd, sizeof(cmd),
        "git branch --contains %s | wc -l", commit);
    FILE *fp = popen(cmd, "r");
    int branches = 0;
    fscanf(fp, "%d", &branches);
    pclose(fp);

    if (branches > 1)
        die("commit is shared by multiple branches");

    /* refuse if commit is pushed */
    snprintf(cmd, sizeof(cmd),
        "git branch -r --contains %s | wc -l", commit);
    fp = popen(cmd, "r");
    int remotes = 0;
    fscanf(fp, "%d", &remotes);
    pclose(fp);

    if (remotes > 0)
        die("commit already pushed (refusing)");

    /* read old message */
    snprintf(cmd, sizeof(cmd), "git show -s --format=%%B %s", commit);
    fp = popen(cmd, "r");
    fread(msg, 1, sizeof(msg)-1, fp);
    pclose(fp);

    char *old_body = strchr(msg, '\n');
    if (old_body && old_body[1] == '\n')
        old_body += 2;
    else if (old_body)
        old_body += 1;
    else
        old_body = "";

    /* write new message */
    FILE *out = fopen(".git-newmsg", "w");
    fprintf(out, "%s\n\n", subject);
    if (body_override)
        fprintf(out, "%s\n", body_override);
    else
        fprintf(out, "%s", old_body);
    fclose(out);

    /* check root */
    snprintf(cmd, sizeof(cmd),
        "git rev-list --parents -n 1 %s | wc -w", commit);
    fp = popen(cmd, "r");
    int words = 0;
    fscanf(fp, "%d", &words);
    pclose(fp);

    /* sequence editor */
    FILE *seq = fopen(".git-seq-edit.sh", "w");
    fprintf(seq,
        "#!/bin/sh\n"
        "sed -i '' 's/^pick %s /reword %s /' \"$1\"\n",
        commit, commit);
    fclose(seq);
    run("chmod +x .git-seq-edit.sh");

    if (words == 1)
        snprintf(cmd, sizeof(cmd),
            "GIT_SEQUENCE_EDITOR=./.git-seq-edit.sh "
            "GIT_EDITOR='cp .git-newmsg' "
            "git rebase -i --root");
    else
        snprintf(cmd, sizeof(cmd),
            "GIT_SEQUENCE_EDITOR=./.git-seq-edit.sh "
            "GIT_EDITOR='cp .git-newmsg' "
            "git rebase -i %s^", commit);

    run(cmd);

    remove(".git-newmsg");
    remove(".git-seq-edit.sh");

    return 0;
}

