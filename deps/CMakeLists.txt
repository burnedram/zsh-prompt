project(zsh-promt-deps)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

INCLUDE("${CMAKE_ROOT}/Modules/ExternalProject.cmake")

EXTERNALPROJECT_ADD(libssl
    #URL https://www.openssl.org/source/openssl-1.1.0j.tar.gz
    URL ${CMAKE_SOURCE_DIR}/openssl-1.1.0j.tar.gz
    URL_MD5 b4ca5b78ae6ae79da80790b30dbedbdc
    CONFIGURE_COMMAND cp -a ../libssl/. ./ COMMAND ./config --prefix=${CMAKE_SOURCE_DIR} --openssldir=${CMAKE_SOURCE_DIR} no-shared
    BUILD_COMMAND unset MFLAGS && unset MAKEFLAGS && unset MAKELEVEL && $(MAKE) build_libs
    INSTALL_COMMAND unset MFLAGS && unset MAKEFLAGS && unset MAKELEVEL && $(MAKE) install_dev
    TEST_COMMAND ""
)

EXTERNALPROJECT_ADD(libz
    #URL https://zlib.net/zlib-1.2.11.tar.gz
    URL ${CMAKE_SOURCE_DIR}/zlib-1.2.11.tar.gz
    URL_MD5 1c9f62f0778697a09d36121ead88e08e
    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}
        # libz is borken and points to share/pkgconfig by default
        -DINSTALL_PKGCONFIG_DIR=${CMAKE_SOURCE_DIR}/lib/pkgconfig
)

EXTERNALPROJECT_ADD(libssh2
    DEPENDS libssl
    DEPENDS libz
    #URL https://github.com/libssh2/libssh2/releases/download/libssh2-1.8.0/libssh2-1.8.0.tar.gz
    URL ${CMAKE_SOURCE_DIR}/libssh2-1.8.0.tar.gz
    URL_MD5 3d1147cae66e2959ea5441b183de1b1c
    CMAKE_ARGS
        -DPKG_CONFIG_USE_CMAKE_PREFIX_PATH=ON
        -DCMAKE_PREFIX_PATH=${CMAKE_SOURCE_DIR}
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_EXAMPLES=OFF
        -DBUILD_TESTING=OFF
        -DCMAKE_BUILD_TYPE=Release
        -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}
)

EXTERNALPROJECT_ADD(libcurl
    DEPENDS libssl
    DEPENDS libz
    #URL https://github.com/curl/curl/releases/download/curl-7_63_0/curl-7.63.0.tar.gz
    URL ${CMAKE_SOURCE_DIR}/curl-7.63.0.tar.gz
    URL_MD5 6121427a7199cd6094fc48c9e31e8992
    CMAKE_ARGS
        -DPKG_CONFIG_USE_CMAKE_PREFIX_PATH=ON
        -DCMAKE_PREFIX_PATH=${CMAKE_SOURCE_DIR}
        -DCURL_ZLIB=ON
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_CURL_EXE=OFF
        -DENABLE_MANUAL=OFF
        -DBUILD_TESTING=OFF
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
        -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}
)

EXTERNALPROJECT_ADD(libgit2
    DEPENDS libz
    DEPENDS libssh2
    DEPENDS libcurl
    #URL https://github.com/libgit2/libgit2/archive/v0.27.7.tar.gz
    URL ${CMAKE_SOURCE_DIR}/libgit2-0.27.7.tar.gz
    URL_MD5 5f3ef881ba0444254f765a2f0a1f3479
    CMAKE_ARGS
        -DPKG_CONFIG_USE_CMAKE_PREFIX_PATH=ON
        -DCMAKE_PREFIX_PATH=${CMAKE_SOURCE_DIR}
        -DBUILD_SHARED_LIBS=OFF 
        -DCMAKE_BUILD_TYPE=Release
        -DBUILD_CLAR=OFF
        -DTHREADSAFE=OFF
        -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}
)
