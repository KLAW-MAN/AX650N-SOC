#!/bin/sh
# Copyright (c) 2019-2024 Axera Semiconductor Co., Ltd. All Rights Reserved.
#
# This source file is the property of Axera Semiconductor Co., Ltd. and
# may not be copied or distributed in any isomorphic form without the prior
# written consent of Axera Semiconductor Co., Ltd.
#
# Author: wanglusheng@axera-tech.com
#
# Run this script to build protobuf from source.  This script is intended to be
# run from the root of the distribution tree.

set -e

# Check that we're being run from the right directory.
if test ! -f src/google/protobuf/stubs/common.h; then
  cat >&2 << __EOF__
Could not find source code.  Make sure you are running this script from the
root of the distribution tree.
__EOF__
  exit 1
fi

# Build the protobuf compiler and runtime from source.
# Note: 'protobuf_WITH_ZLIB' can be set to 'ON' to enable zlib support.
mkdir -p build_aarch64 && pushd build_aarch64
cmake -DCMAKE_BUILD_TYPE=Release                        \
      -DCMAKE_SYSTEM_NAME=Linux                         \
      -DCMAKE_SYSTEM_PROCESSOR=aarch64                  \
      -DCMAKE_C_COMPILER="aarch64-none-linux-gnu-gcc"   \
      -DCMAKE_CXX_COMPILER="aarch64-none-linux-gnu-g++" \
      -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER         \
      -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY          \
      -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY          \
      -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON         \
      -Dprotobuf_BUILD_PROTOC_BINARIES:BOOL=ON          \
      -Dprotobuf_BUILD_SHARED_LIBS:BOOL=OFF             \
      -Dprotobuf_BUILD_TESTS:BOOL=OFF                   \
      -Dprotobuf_DISABLE_RTTI:BOOL=ON                   \
      -Dprotobuf_INSTALL:BOOL=ON                        \
      -Dprotobuf_WITH_ZLIB:BOOL=OFF                     \
      -DCMAKE_INSTALL_PREFIX=../install/arm64          \
      ../cmake
cmake --build . --target install
popd

mkdir -p build_riscv && pushd build_riscv
cmake -DCMAKE_BUILD_TYPE=Release                        \
      -DCMAKE_SYSTEM_NAME=Linux                         \
      -DCMAKE_SYSTEM_PROCESSOR=riscv                    \
      -DCMAKE_C_COMPILER="riscv64-linux-gnu-gcc"        \
      -DCMAKE_CXX_COMPILER="riscv64-linux-gnu-g++"      \
      -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER         \
      -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY          \
      -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY          \
      -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON         \
      -Dprotobuf_BUILD_PROTOC_BINARIES:BOOL=ON          \
      -Dprotobuf_BUILD_SHARED_LIBS:BOOL=OFF             \
      -Dprotobuf_BUILD_TESTS:BOOL=OFF                   \
      -Dprotobuf_DISABLE_RTTI:BOOL=ON                   \
      -Dprotobuf_INSTALL:BOOL=ON                        \
      -Dprotobuf_WITH_ZLIB:BOOL=OFF                     \
      -DCMAKE_INSTALL_PREFIX=../install/riscv           \
      ../cmake
cmake --build . --target install
popd

mkdir -p build_x64 && pushd build_x64
cmake -DCMAKE_BUILD_TYPE=Release                \
      -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON \
      -Dprotobuf_BUILD_PROTOC_BINARIES:BOOL=ON  \
      -Dprotobuf_BUILD_SHARED_LIBS:BOOL=OFF     \
      -Dprotobuf_BUILD_TESTS:BOOL=OFF           \
      -Dprotobuf_DISABLE_RTTI:BOOL=ON           \
      -Dprotobuf_INSTALL:BOOL=ON                \
      -Dprotobuf_WITH_ZLIB:BOOL=OFF             \
      -DCMAKE_INSTALL_PREFIX=../install/x64   \
      ../cmake
cmake --build . --target install
popd
