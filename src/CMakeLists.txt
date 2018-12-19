PROJECT(zsh-prompt C)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

# Versioning
SET(zsh-prompt_VERSION_MAJOR 1)
SET(zsh-prompt_VERSION_MINOR 0)
CONFIGURE_FILE(
    "${zsh-prompt_SOURCE_DIR}/config.h.in"
    "${zsh-prompt_BINARY_DIR}/config.h"
)
INCLUDE_DIRECTORIES("${zsh-prompt_BINARY_DIR}")

INCLUDE_DIRECTORIES("${DEPS_INCLUDE_DIR}")
LINK_DIRECTORIES("${DEPS_LIB_DIR}")

ADD_EXECUTABLE(zsh-prompt zsh-prompt.c)
TARGET_LINK_LIBRARIES(zsh-prompt libgit2.a libz.a libcurl.a libssh2.a libssl.a libcrypto.a dl pthread)

INSTALL (TARGETS zsh-prompt DESTINATION bin)
