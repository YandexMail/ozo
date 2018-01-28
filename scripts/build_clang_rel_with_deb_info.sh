#!/bin/bash -ex

export CC='ccache clang'
export CXX='ccache clang++'

BUILD_DIR=${BUILD_PREFIX}build_clang_rel_with_deb_info
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}
cmake \
    -DCMAKE_CXX_FLAGS='-std=c++17 -Wall -Wextra -pedantic -Werror' \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DOZO_BUILD_TESTS=ON \
    -DPostgreSQL_TYPE_INCLUDE_DIR=/usr/include/postgresql/ \
    ..
make -j$(nproc)
ctest -V
