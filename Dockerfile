FROM debian:stretch

RUN apt-get update
RUN apt-get install -y \
    gcc \
    g++ \
    make \
    cmake \
    pkg-config \
    libfindbin-libs-perl

WORKDIR /root
COPY deps deps

RUN mkdir deps/build
WORKDIR /root/deps/build
RUN cmake ..
RUN cmake --build .

WORKDIR /root
RUN rm -rf deps/build
COPY CMakeLists.txt CMakeLists.txt
COPY src src

RUN mkdir build
WORKDIR /root/build
RUN cmake ..
RUN cmake --build .
