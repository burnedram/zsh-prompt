FROM debian:stretch

WORKDIR /root
COPY Makefile Makefile
COPY CMakeLists.txt CMakeLists.txt
COPY src src

RUN apt-get update
RUN apt-get install -y \
    gcc \
    g++ \
    make \
    cmake \
    pkg-config \
    libfindbin-libs-perl
