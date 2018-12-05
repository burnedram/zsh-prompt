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

int get_branch(git_reference **out_ref, git_repository *repo, const git_oid *oid) {
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
        if (!git_oid_cmp(oid, branch_oid)) {
            error = git_reference_resolve(out_ref, branch_ref);
            git_reference_free(branch_ref);
            break;
        }
        git_reference_free(branch_ref);
    }

    git_branch_iterator_free(branches);
    return error;
}

int get_odb_data(git_odb_object **out_object, const void **out_data, size_t *out_size, git_odb *odb, const git_oid *oid) {
    if (!git_odb_exists(odb, oid))
        return GIT_ENOTFOUND;

    int error;
    error = git_odb_read(out_object, odb, oid);
    if (error)
        return error;

    if (out_size != NULL)
        *out_size = git_odb_object_size(*out_object);
    *out_data = git_odb_object_data(*out_object);

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

            const git_oid *head_oid = git_reference_target(head_ref);
            char head_shortsha[9] = {0};
            git_oid_tostr(head_shortsha, 8, head_oid);
            printf("HEAD: %s\n", head_shortsha);

            {
                git_reference *branch_ref = NULL;
                error = get_branch(&branch_ref, repo, head_oid);
                if (error)
                    die_giterr(error);

                const char *branch_name = NULL;
                error = git_branch_name(&branch_name, branch_ref);
                if (error)
                    die_giterr(error);
                printf("Branch %s\n", branch_name);

                {
                    git_reference *upstream_ref = NULL;
                    {
                        git_reference *upstream_ref_unresolved = NULL;
                        error = git_branch_upstream(&upstream_ref_unresolved, branch_ref);
                        if (error != GIT_ENOTFOUND) {
                            if (error)
                                die_giterr(error);
                            error = git_reference_resolve(&upstream_ref, upstream_ref_unresolved);
                            if (error)
                                die_giterr(error);
                            git_reference_free(upstream_ref_unresolved);
                        }
                    }

                    if (!upstream_ref)
                        printf("No remote tracking branch\n");
                    else {
                        const git_oid *upstream_oid = git_reference_target(upstream_ref);
                        char upstream_shortsha[9] = {0};
                        git_oid_tostr(upstream_shortsha, 8, upstream_oid);
                        printf("Upstream OID: %s\n", upstream_shortsha);

                        const char *upstream_name = NULL;
                        error = git_branch_name(&upstream_name, upstream_ref);
                        if (error)
                            die_giterr(error);
                        printf("Upstream name: %s\n", upstream_name);

                        {
                            git_revwalk *walker = NULL;
                            error = git_revwalk_new(&walker, repo);
                            if (error)
                                die_giterr(error);
                            error = git_revwalk_push(walker, head_oid);
                            if (error)
                                die_giterr(error);
                            error = git_revwalk_hide(walker, upstream_oid);
                            if (error)
                                die_giterr(error);
                            git_oid walker_oid = {0};
                            char walker_shortsha[9] = {0};

                            size_t commits_ahead = 0;
                            printf("====Head -> Remote====\n");
                            while (!(error = git_revwalk_next(&walker_oid, walker))) {
                                commits_ahead++;
                                git_oid_tostr(walker_shortsha, 8, &walker_oid);
                                printf("%s\n", walker_shortsha);
                            }
                            if (error != GIT_ITEROVER)
                                die_giterr(error);

                            git_revwalk_reset(walker);
                            error = git_revwalk_push(walker, upstream_oid);
                            if (error)
                                die_giterr(error);
                            error = git_revwalk_hide(walker, head_oid);
                            if (error)
                                die_giterr(error);

                            size_t commits_behind = 0;
                            printf("====Remote -> Head====\n");
                            while (!(error = git_revwalk_next(&walker_oid, walker))) {
                                commits_behind++;
                                git_oid_tostr(walker_shortsha, 8, &walker_oid);
                                printf("%s\n", walker_shortsha);
                            }
                            if (error != GIT_ITEROVER)
                                die_giterr(error);

                            printf("======================\n");
                            printf("%d commits ahead\n", commits_ahead);
                            printf("%d commits behind\n", commits_behind);

                            git_revwalk_free(walker);
                        }

                        git_reference_free(upstream_ref);
                    }
                }

                git_reference_free(branch_ref);
            }

            {
                git_odb *odb = NULL;
                error = git_repository_odb(&odb, repo);
                if (error)
                    die_giterr(error);

                git_odb_object *object = NULL;
                const void *data = NULL;
                error = get_odb_data(&object, &data, NULL, odb, head_oid);
                if (error)
                    die_giterr(error);
                printf("====ODB data====\n%s================\n", data);

                git_odb_object_free(object);
                git_odb_free(odb);
            }
            git_reference_free(head_ref);
        }
        git_repository_free(repo);
    }

    git_libgit2_shutdown();
    return 0;
}
