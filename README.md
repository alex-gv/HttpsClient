[![Build Status](https://github.com/alex-gv/https_client/actions/workflows/build.yml/badge.svg?branch=main)](https://github.com/alex-gv/https_client/actions/workflows/build.yml)

# HttpsClient

## 📌 Description

**HttpsClient** is a cross-platform C++ library for performing HTTPS requests.
It allows you to send GET/POST requests, receive responses from HTTPS servers, and integrates easily into CMake projects.


## 🚀 Features

✨ HTTPS/SSL support
✨ Simple API for sending requests
✨ Integration with **CMake** and **vcpkg**
✨ Automated build and CI via GitHub Actions

## 📦 Installation and Build

### 🧱 Requirements

- C++17 (or newer)
- CMake 3.15+
- OpenSSL (or another SSL backend)
- (Optional) **vcpkg** for dependency management

### 🛠 Building with CMake

```bash
git clone https://github.com/alex-gv/HttpsClient.git
cd HttpsClient

cmake -B build -S . \
  -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake \
  -DCMAKE_BUILD_TYPE=Release

cmake --build build --config Release
```

🧪 Testing
```bash
cd build
ctest --output-on-failure
```


📜 License

This project is licensed under the MIT License.