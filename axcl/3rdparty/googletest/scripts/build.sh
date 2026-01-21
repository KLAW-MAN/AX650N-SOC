set -e

mkdir -p build_aarch64 && pushd build_aarch64
cmake -DCMAKE_BUILD_TYPE=Release                        \
      -DCMAKE_SYSTEM_NAME=Linux                         \
      -DCMAKE_SYSTEM_PROCESSOR=aarch64                  \
      -DCMAKE_C_COMPILER="aarch64-none-linux-gnu-gcc"   \
      -DCMAKE_CXX_COMPILER="aarch64-none-linux-gnu-g++" \
      -DCMAKE_INSTALL_PREFIX=../install/arm64           \
      ..
cmake --build . --target install
popd

mkdir -p build_riscv && pushd build_riscv
cmake -DCMAKE_BUILD_TYPE=Release                      \
      -DCMAKE_SYSTEM_NAME=Linux                       \
      -DCMAKE_SYSTEM_PROCESSOR=riscv                  \
      -DCMAKE_C_COMPILER="riscv64-linux-gnu-gcc"        \
      -DCMAKE_CXX_COMPILER="riscv64-linux-gnu-g++"      \
      -DCMAKE_INSTALL_PREFIX=../install/riscv         \
      ..
cmake --build . --target install
popd

mkdir -p build_x64 && pushd build_x64
cmake -DCMAKE_BUILD_TYPE=Release                \
      -DCMAKE_INSTALL_PREFIX=../install/x64     \
      ..
cmake --build . --target install
popd


# windows
#   1 googletest\cmake\internal_utils.cmake: remove -DUNICODE -D_UNICODE
#   2 set_target_properties(gtest PROPERTIES PREFIX "lib")
#   3 set_target_properties(gtest_main PROPERTIES PREFIX "lib")
#   4 set_target_properties(gmock PROPERTIES PREFIX "lib")
#   5 set_target_properties(gmock_main PROPERTIES PREFIX "lib")
#   6 gtest_force_shared_crt ON
#
# cmake -S .. -B . -G "Visual Studio 17 2022" \
# -DCMAKE_BUILD_TYPE=Release \
# -DCMAKE_STATIC_LIBRARY_PREFIX="lib"   \
# -DCMAKE_CXX_STANDARD=17 \
# -DCMAKE_INSTALL_PREFIX="./install"
# cmake --build . --config Release
# cmake --install . --config Release
