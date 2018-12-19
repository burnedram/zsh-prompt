CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

INCLUDE("${CMAKE_ROOT}/Modules/ExternalProject.cmake")

EXTERNALPROJECT_ADD(libz
    URL https://zlib.net/zlib-1.2.11.tar.gz
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/deps
        # libz is borken and points to share/pkgconfig by default
        -DINSTALL_PKGCONFIG_DIR=${CMAKE_BINARY_DIR}/deps/lib/pkgconfig
)

EXTERNALPROJECT_ADD(libssh2
    DEPENDS libz
    URL https://github.com/libssh2/libssh2/releases/download/libssh2-1.8.0/libssh2-1.8.0.tar.gz
    CMAKE_ARGS
        -DPKG_CONFIG_USE_CMAKE_PREFIX_PATH=ON
        -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/deps
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/deps
)

EXTERNALPROJECT_ADD(libcurl
    DEPENDS libz
    URL https://github.com/curl/curl/releases/download/curl-7_63_0/curl-7.63.0.tar.gz
    CMAKE_ARGS
        -DPKG_CONFIG_USE_CMAKE_PREFIX_PATH=ON
        -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/deps
        -DCURL_LIBZ=ON
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_CURL_EXE=OFF
        -DENABLE_ARES=OFF
        -DCURL_DISABLE_FTP=ON
        -DCURL_DISABLE_LDAP=ON
        -DCURL_DISABLE_TELNET=ON
        -DCURL_DISABLE_DICT=ON
        -DCURL_DISABLE_FILE=ON
        -DCURL_DISABLE_TFTP=ON
        -DCURL_DISABLE_RTSP=ON
        -DCURL_DISABLE_POP3=ON
        -DCURL_DISABLE_IMAP=ON
        -DCURL_DISABLE_SMTP=ON
        -DCURL_DISABLE_GOPHER=ON
        -DCMAKE_USE_LIBSSH2=OFF
        -DENABLE_UNIX_SOCKETS=OFF
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/deps
)

EXTERNALPROJECT_ADD(libgit2
    DEPENDS libz
    DEPENDS libssh2
    DEPENDS libcurl
    URL https://github.com/libgit2/libgit2/archive/v0.27.7.tar.gz
    CMAKE_ARGS
        -DPKG_CONFIG_USE_CMAKE_PREFIX_PATH=ON
        -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}/deps
        -DBUILD_SHARED_LIBS=OFF 
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_CLAR=OFF
        -DTHREADSAFE=OFF
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/deps
)

EXTERNALPROJECT_ADD(zsh-prompt
    DEPENDS libgit2
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/src
    CMAKE_ARGS
        -DDEPS_INCLUDE_DIR=${CMAKE_BINARY_DIR}/deps/include
        -DDEPS_LIB_DIR=${CMAKE_BINARY_DIR}/deps/lib
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}
)
