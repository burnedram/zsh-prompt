#include <stdio.h>
#include <git2.h>
#include "config.h"

int main() {
    git_libgit2_init();
    printf("Hello world! %s v%d.%d\n", PROJECT_NAME, PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR);
    git_libgit2_shutdown();
    return 0;
}
