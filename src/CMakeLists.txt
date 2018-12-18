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

INCLUDE_DIRECTORIES("${LIBSSH2_INSTALL_PATH}/include")
LINK_DIRECTORIES("${LIBSSH2_INSTALL_PATH}/lib")
INCLUDE_DIRECTORIES("${LIBGIT2_INSTALL_PATH}/include")
LINK_DIRECTORIES("${LIBGIT2_INSTALL_PATH}/lib")

ADD_EXECUTABLE(zsh-prompt zsh-prompt.c)
TARGET_LINK_LIBRARIES(zsh-prompt libgit2.a libssh2.a libssl.a libcrypto.a curl z dl gcrypt)

INSTALL (TARGETS zsh-prompt DESTINATION bin)
