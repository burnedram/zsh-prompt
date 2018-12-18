CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

INCLUDE("${CMAKE_ROOT}/Modules/ExternalProject.cmake")

EXTERNALPROJECT_ADD(libssh2
    URL https://github.com/libssh2/libssh2/releases/download/libssh2-1.8.0/libssh2-1.8.0.tar.gz
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libssh2
)

EXTERNALPROJECT_ADD(libgit2
    URL https://github.com/libgit2/libgit2/archive/v0.27.7.tar.gz
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF 
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_CLAR=OFF
        -DTHREADSAFE=OFF
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/libgit2
)

EXTERNALPROJECT_ADD(zsh-prompt
    DEPENDS libgit2
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=Release
        -DLIBSSH2_INSTALL_PATH=${CMAKE_BINARY_DIR}/libssh2
        -DLIBGIT2_INSTALL_PATH=${CMAKE_BINARY_DIR}/libgit2
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
)
