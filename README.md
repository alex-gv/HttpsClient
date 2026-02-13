[![CMake with vcpkg and cache](https://github.com/alex-gv/https_client/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/alex-gv/https_client/actions/workflows/build.yml)

#build
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=C:/vcpkg/scripts/buildsystems/vcpkg.cmake  -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release