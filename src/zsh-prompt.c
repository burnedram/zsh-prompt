#include <stdio.h>
#include <git2.h>
#include "config.h"

void die_giterr(int error) {
    const git_error *e = giterr_last();
    printf("Error %d/%d: %s\n", error, e->klass, e->message);
    exit(error);
}

int get_head(git_reference **out_ref, git_repository *repo) {
    int error;
    git_reference *head_ref = NULL;
    error = git_repository_head(&head_ref, repo);
    if (error)
        return error;
    error = git_reference_resolve(out_ref, head_ref);
    git_reference_free(head_ref);
    return error;
}

int get_branch_name(const char **out_name, git_repository *repo, const git_oid *oid) {
    int error;
    git_branch_iterator *branches = NULL;
    error = git_branch_iterator_new(&branches, repo, GIT_BRANCH_LOCAL);
    if (error)
        return error;

    git_reference *branch_ref = NULL;
    git_branch_t branch_type;
    const git_oid *branch_oid = NULL;
    while (!(error = git_branch_next(&branch_ref, &branch_type, branches))) {
        branch_oid = git_reference_target(branch_ref);
        if (git_oid_cmp(oid, branch_oid))
            continue;

        error = git_branch_name(out_name, branch_ref);
        break;
    }

    git_branch_iterator_free(branches);
    return error;
}

int get_odb_data(const void **out_data, size_t *out_size, git_odb *odb, const git_oid *oid) {
    if (!git_odb_exists(odb, oid))
        return GIT_ENOTFOUND;

    int error;
    git_odb_object *odb_object = NULL;
    error = git_odb_read(&odb_object, odb, oid);
    if (error)
        return error;

    if (out_size != NULL)
        *out_size = git_odb_object_size(odb_object);
    *out_data = git_odb_object_data(odb_object);

    git_odb_object_free(odb_object);
    return error;
}

int main() {
    git_libgit2_init();
    printf("Hello world! %s v%d.%d\n", PROJECT_NAME, PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR);
    int error;

    {
        git_repository *repo = NULL;
        error = git_repository_open(&repo, ".");
        if (error == GIT_ENOTFOUND) {
            printf("Not in a git repository\n");
            exit(0);
        } else if (error)
            die_giterr(error);

        {
            git_reference *head_ref = NULL;
            error = get_head(&head_ref, repo);
            if (error)
                die_giterr(error);

            const git_oid *head_oid = NULL;
            head_oid = git_reference_target(head_ref);
            char head_shortsha[9] = {0};
            git_oid_tostr(head_shortsha, 8, head_oid);
            printf("Commit %s\n", head_shortsha);

            const char *branch_name = NULL;
            error = get_branch_name(&branch_name, repo, head_oid);
            if (error)
                die_giterr(error);
            printf("Branch %s\n", branch_name);

            {
                git_odb *odb = NULL;
                error = git_repository_odb(&odb, repo);
                if (error)
                    die_giterr(error);

                const void *data = NULL;
                error = get_odb_data(&data, NULL, odb, head_oid);
                if (error)
                    die_giterr(error);
                printf("====ODB data====\n%s================\n", data);

                git_odb_free(odb);
            }
            git_reference_free(head_ref);
        }
        git_repository_free(repo);
    }

    git_libgit2_shutdown();
    return 0;
}
