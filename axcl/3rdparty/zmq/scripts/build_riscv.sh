#!/usr/bin/env bash

# Note: should using 4.3.5, 4.3.4 cannot pass build when gcc is 13 (riscv requested)

# get root path
work_root="$(cd "$(dirname "${BASH_SOURCE[0]}")"  && pwd)"

# make release build folder
mkdir -p build-riscv || (echo "Building release folder is already existed." && exit)

# cmake debug configure
cmake -S ${work_root} -B ${work_root}/build-riscv            \
      -DCMAKE_MAKE_PROGRAM="$(which make)"                   \
      -DCMAKE_BUILD_TYPE="RELEASE"                           \
      -DCMAKE_SYSTEM_NAME=Linux                              \
      -DCMAKE_SYSTEM_PROCESSOR=riscv64                       \
      -DCMAKE_C_COMPILER=riscv64-unknown-linux-gnu-gcc       \
      -DCMAKE_CXX_COMPILER=riscv64-unknown-linux-gnu-g++     \
      -DCMAKE_FIND_ROOT_PATH_MODE_PROGRAM=NEVER              \
      -DCMAKE_FIND_ROOT_PATH_MODE_LIBRARY=ONLY               \
      -DCMAKE_FIND_ROOT_PATH_MODE_INCLUDE=ONLY               \
      -DCMAKE_INSTALL_PREFIX=${work_root}/install/riscv      \
      || (popd && echo "Configure building failed." && exit)

# build
cmake --build ${work_root}/build-riscv --parallel "$(nproc)" && cmake --install ${work_root}/build-riscv --strip
