FROM ubuntu:18.04

RUN apt-get update && \
    apt-get install -y apt-transport-https ca-certificates gnupg software-properties-common wget

RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | apt-key add -

RUN apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'

RUN apt-get update && \
    apt-get install -y \
        ccache \
        clang-7 \
        curl \
        g++-8 \
        git \
        gdb \
        libpq-dev \
        llvm-7 \
        postgresql-client \
        postgresql-server-dev-all \
        python-openssl \
        python-pip \
        python-yaml \
        python3-pip \
        cmake \
        doxygen \
        && \
    rm -rf /var/lib/apt/lists/*

RUN update-alternatives --install /usr/bin/clang clang /usr/bin/clang-7 100 && \
    update-alternatives --install /usr/bin/clang++ clang++ /usr/bin/clang++-7 100 && \
    update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-8 100 && \
    update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-8 100 && \
    update-alternatives --install /usr/bin/gcov gcov /usr/bin/gcov-8 100 && \
    update-alternatives --install /usr/bin/llvm-symbolizer llvm-symbolizer /usr/bin/llvm-symbolizer-7 100

RUN wget -qO boost_1_66_0.tar.gz https://boostorg.jfrog.io/artifactory/main/release/1.66.0/source/boost_1_66_0.tar.gz && \
    tar xzf boost_1_66_0.tar.gz && \
    cd boost_1_66_0 && \
    ./bootstrap.sh --with-libraries=atomic,system,thread,chrono,date_time,context,coroutine,program_options && \
    ./b2 \
        -j $(nproc) \
        --reconfigure \
        link=static \
        threading=multi \
        variant=release \
        cxxflags='-std=c++17 -DBOOST_COROUTINES_NO_DEPRECATION_WARNING' \
        debug-symbols=on \
        warnings=off \
    install

RUN pip install gcovr && \
    pip3 install conan

RUN wget -qO /codecov.sh https://codecov.io/bash && chmod +x /codecov.sh

VOLUME /ccache
VOLUME /code

WORKDIR /code

ENV CCACHE_DIR=/ccache
