#include <stdio.h>
#include <git2.h>
#include "config.h"

void die_giterr(int error) {
    const git_error *e = giterr_last();
    printf("Error %d/%d: %s\n", error, e->klass, e->message);
    exit(error);
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
            git_reference *head_refdirect = NULL;
            {
                git_reference *head_ref = NULL;
                error = git_repository_head(&head_ref, repo);
                if (error)
                    die_giterr(error);
                error = git_reference_resolve(&head_refdirect, head_ref);
                if (error)
                    die_giterr(error);
                git_reference_free(head_ref);
            }

            const git_oid *head_oid = NULL;
            head_oid = git_reference_target(head_refdirect);
            char head_shortsha[9] = {0};
            git_oid_tostr(head_shortsha, 8, head_oid);
            printf("Commit %s\n", head_shortsha);

            {
                git_branch_iterator *branches = NULL;
                error = git_branch_iterator_new(&branches, repo, GIT_BRANCH_LOCAL);
                if (error)
                    die_giterr(error);

                git_reference *branch_ref = NULL;
                git_reference *branch_refdirect = NULL;
                git_branch_t branch_type;
                const git_oid *branch_oid = NULL;
                const char *branch_name = NULL;
                while (!(error = git_branch_next(&branch_ref, &branch_type, branches))) {
                    error = git_branch_name(&branch_name, branch_ref);
                    if (error)
                        die_giterr(error);
                    printf("Branch %s\n", branch_name);
                    branch_oid = git_reference_target(branch_ref);
                    if (!git_oid_cmp(head_oid, branch_oid)) {
                        printf("^^^^ Current branch!\n");
                    }
                }
                if (error != GIT_ITEROVER)
                    die_giterr(error);

                git_branch_iterator_free(branches);
            }

            {
                git_odb *odb = NULL;
                error = git_repository_odb(&odb, repo);
                if (error)
                    die_giterr(error);

                if (!git_odb_exists(odb, head_oid))
                    printf("Detached\n");
                else {
                    git_odb_object *odb_object = NULL;
                    error = git_odb_read(&odb_object, odb, head_oid);
                    if (error)
                        die_giterr(error);

                    size_t data_size = git_odb_object_size(odb_object);
                    const void *data = git_odb_object_data(odb_object);
                    printf("====ODB data====\n%.*s================\n", data_size, data);

                    git_odb_object_free(odb_object);
                }

                git_odb_free(odb);
            }
            git_reference_free(head_refdirect);
        }
        git_repository_free(repo);
    }

    git_libgit2_shutdown();
    return 0;
}
