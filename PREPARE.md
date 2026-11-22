# Prepare Build Environment

> Only focussing on Linux since Docker is not available on Windows as it is on Linux with mounting
> local volumes.

### Extract and Install the Library

Run this command to install enable building with 5 different toolchains and
generate Doxygen documentation.

```shell
./sf-cmake.py --extract --install
```

### Install Docker when not installed (Ubuntu/Debian)

Install the latest Docker-CE package when Docker is not installed already.

```shell
./build.py install --required dce
```

### Building using Docker

Build the project using Docker with Linux GNU x86_64 toolchain.

```shell
./build.py docker --build-dir tmp -- --build gnu-debug
```

Build the project using Docker with Linux GNU aarch64 cross-compiler toolchain.

```shell
./build.py docker --build-dir tmp -- --build ga-debug
```

Build the project using Docker with Windows MSVC toolchain in Wine.

```shell
./build.py docker --build-dir tmp -- wine -- --build msvc-debug
```

### Build Doxygen Documentation

```shell
./build.py d -b tmp -- -b gnu-debug -n document
```

### Show Preset Information

Show build options for all toolchains under Linux and Wine.

```shell
./build.py docker -b tmp -- --info
```

```shell
./build.py docker -b tmp -- wine -- --info 
```

### Package the Project

Package the **debug** version.

```shell
./build.py docker -b tmp -- -bp gnu-debug 
```

Package the **release** version.

```shell
./build.py docker -b tmp -- -bp gnu-release 
```

Package the MSVC **debug** version.

```shell
./build.py docker -b tmp -- w -- -bp msvc-release
```
