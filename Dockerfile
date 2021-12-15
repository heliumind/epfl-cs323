FROM ubuntu:20.04

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && \
    apt-get install -y \
    gcc gcc-multilib g++ g++-multilib build-essential
RUN apt-get install -y clang
RUN apt-get install -y --no-install-recommends tzdata
RUN apt-get install -y expect zlib1g-dev autoconf automake libtool
