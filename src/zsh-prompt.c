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

int cred(git_cred **out, const char *url, const char *username_from_url, unsigned int allowed_types, void *payload) {
    return git_cred_ssh_key_from_agent(out, username_from_url);
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

        git_buf remote_name = {0};

        {
            git_reference *head_ref = NULL;
            error = get_head(&head_ref, repo);
            if (error)
                die_giterr(error);

            const git_oid *head_oid = git_reference_target(head_ref);
            char head_shortsha[9] = {0};
            git_oid_tostr(head_shortsha, 8, head_oid);
            printf("HEAD: %s\n", head_shortsha);
            
            if (git_reference_is_branch(head_ref)) {
                const char *branch_shorthand = NULL;
                branch_shorthand = git_reference_shorthand(head_ref);
                printf("Branch %s\n", branch_shorthand);

                {
                    git_reference *branch_ref = NULL;
                    error = git_branch_lookup(&branch_ref, repo, branch_shorthand, GIT_BRANCH_LOCAL);
                    if (error)
                        die_giterr(error);

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

                            error = git_branch_remote_name(&remote_name, repo, git_reference_name(upstream_ref));
                            if (error)
                                die_giterr(error);
                            printf("Upstream remote name: %s\n", remote_name.ptr);

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

        {
            git_status_list *status_list = NULL;
            git_status_options opts = GIT_STATUS_OPTIONS_INIT;
            opts.show = GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
            opts.flags |= GIT_STATUS_OPT_EXCLUDE_SUBMODULES;
            opts.flags |= GIT_STATUS_OPT_INCLUDE_UNTRACKED;
            error = git_status_list_new(&status_list, repo, &opts);
            if (error)
                die_giterr(error);

            size_t entrycount = git_status_list_entrycount(status_list);
            const git_status_entry *status_entry = NULL;
            char staged = 0, unstaged = 0, untracked = 0;
            for (size_t i = 0; i < entrycount && (!staged || !unstaged || !untracked); i++) {
                status_entry = git_status_byindex(status_list, i);
                if (status_entry->status == GIT_STATUS_CURRENT)
                    continue;

                if (status_entry->head_to_index) {
                    staged = 1;
                }
                if (status_entry->index_to_workdir) {
                    if (status_entry->status & GIT_STATUS_WT_NEW)
                        untracked = 1;
                    else
                        unstaged = 1;
                }
            }

            printf("====Status====\n");
            if (staged)
                printf("There are staged changes\n");
            if (unstaged)
                printf("There are unstaged changes\n");
            if (untracked)
                printf("There are untracked changes\n");
            if (!staged && !unstaged && !untracked)
                printf("No changes\n");
            printf("==============\n");

            git_status_list_free(status_list);
        }

        {
            git_strarray tag_names = {0};
            error = git_tag_list(&tag_names, repo);
            if (error)
                die_giterr(error);

            printf("====Tags====\n");
            git_object *tag_object = NULL;
            for (size_t i = 0; i < tag_names.count; i++) {
                error = git_revparse_single(&tag_object, repo, tag_names.strings[i]);
                if (error)
                    die_giterr(error);

                if (git_object_type(tag_object) == GIT_OBJ_TAG) {
                    git_object *new_object = NULL;
                    error = git_tag_peel(&new_object, (git_tag *)tag_object);
                    if (error)
                        die_giterr(error);
                    git_object_free(tag_object);
                    tag_object = new_object;
                }

                if (git_object_type(tag_object) != GIT_OBJ_COMMIT) {
                    printf("%s does not point to a commit pls halp\n", tag_names.strings[i]);
                    git_object_free(tag_object);
                    continue;
                }

                git_commit *tag_commit = (git_commit *)tag_object;
                const git_oid *tag_oid = git_commit_id(tag_commit);
                char tag_shortsha[9] = {0};
                git_oid_tostr(tag_shortsha, 8, tag_oid);

                printf("%s %s\n", tag_names.strings[i], tag_shortsha);
                git_object_free(tag_object);
            }
            printf("============\n");
        }

        {
            git_remote *remote = NULL;
            if (remote_name.ptr) {
                error = git_remote_lookup(&remote, repo, remote_name.ptr);

                git_buf_free(&remote_name);
                remote_name.ptr = NULL;
            } else {
                printf("Using default remote name \"origin\"\n");
                error = git_remote_lookup(&remote, repo, "origin");
            }
            if (error)
                die_giterr(error);

            git_remote_callbacks callbacks = GIT_REMOTE_CALLBACKS_INIT;
            callbacks.credentials = cred;
            error = git_remote_connect(remote, GIT_DIRECTION_FETCH, &callbacks, NULL, NULL);
            if (error)
                die_giterr(error);

            const git_remote_head **refs = NULL;
            size_t refs_len;
            error = git_remote_ls(&refs, &refs_len, remote);
            if (error)
                die_giterr(error);

            printf("====Remote refs====\n");
            for (size_t i = 0; i < refs_len; i++) {
                char shortsha[9] = {0};
                git_oid_tostr(shortsha, 8, &refs[i]->oid);
                printf("%s %s\n", shortsha, refs[i]->name);
            }
            printf("===================\n");

            git_remote_free(remote);
        }

        git_repository_free(repo);
    }

    git_libgit2_shutdown();
    return 0;
}
