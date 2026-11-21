# Agent Instructions

## General Coding

- Add comments in code to explain what is to be achieved with the code bellow.
- Comments must be placed above the line of code and NEVER behind.
- The comment can describe a block of code but only small ones.

## Building the Project

The `build.py` script provide environments to for multiple toolchains using CMake presets.

### On Linux

#### Toolchain: GNU

Use the `gnu-debug` preset with a local binary directory.

For building the command is:

```shell
build.py -b gnu-debug
```

For running the command is:

```shell
build.py run -p gnu-debug -- ./AdvancedDockingSystemDemo
```

#### Toolchain: GW

> Toolchain `gw` is the Linux MinGW cross-compiler for Windows.

Use the `gw-debug` preset with a local binary directory.

For building the command is:

```shell
build.py -b gw-debug
```

For running the command is:

```shell
build.py run -p gw-debug -- AdvancedDockingSystemDemo.exe
```

### On Windows

#### Toolchain: MinGW

Use the `l-mingw-debug` preset with a local binary directory.

For building the command is:

```shell
build.py -b l-mingw-debug
```

For running the command is:

```shell
build.py run -p l-mingw-debug -- AdvancedDockingSystemDemo.exe
```

#### Toolchain: MSVC

Use the `l-msvc-debug` preset with a local binary directory.

For building the command is:

```shell
build.py -b l-msvc-debug
```

For running the command is:

```shell
build.py run -p l-msvc-debug -- AdvancedDockingSystemDemo.exe
```

## Restructuring Code

### Code Formatting

#### C++ Files

For formatting the code use the command (on Linux only):

```shell
./check-format.sh --format [<file-or-dir> ...]
```

This file also check for unix line endings.

#### CMake Files

For formatting the code use the command (on Linux only):

```shell
./cmake-format.sh --format [<file-or-dir> ...]
```

This file also check for unix line endings.

### Coding

**General Rules:**

- Defined sentries are never used.
- Pragma `#pragma once` is use as a sentry to prevent multiple inclusions.
- Inline functions are preferred over macros.
- Files suffixes are:
    - `.h`   : File for declarations only
    - `.hpp` : File for inlined definitions (only when the file exists and is never automatically created)
    - `.cpp` : File for definitions
- Inline functions and methods are declared in the class and the definition is below in same file or in the `.hpp` file
  when it exists.
- Header files (`.h`) contains preferable a single class and must be named after the (main-)class name in it.
- when a class method uses keyword `override` the `virtual` keyword is removed.
- Private classes and methods for a class are declared in files named `<classname>_p.*` so they are easily excluded from
  generated documentation and interface file exports.
- Class constructors must be declared at the top of the class when possible.
- Class data members must be declared at the end of rhe class declaration.

**Qt Rules:**

- For Qt use for forward declarations of classes macro `QT_FORWARD_DECLARE_CLASS`.
- For Qt use for forward declarations of structs macro `QT_FORWARD_DECLARE_STRUCT`.

### In-Code Documenting

#### Doc-blocks

Only the declaration of a class and all function in a header contain dock-blocks.
Private classes are also documented even though they are not exported.

The doc-blocks in the code are formatted basically like:

```cpp
/**
 * @brief Exported function from a dynamic library.
 * A different string is returned depending on the value of `how`.
 * @param how Determines what string is returned.
 * @return Resulting string.
 */
std::string getHello(int how);
```

**Rules:**

- Doc-blocks must contain full sentences with a capital letter at the beginning and a period at the end.
- Each function, member function or member data must at least have a doc-block containing a `@brief` entry
- Only class/struct data members can have s `/// @brief` short doc-block.
- When no doc-block exists one is generated.
- When a doc-block for a function or method is found at the definition it is moved to the declaration of it
- Although `\` are allowed preferred is using `@` es the special Doxygen character

Files heading block like:

```cpp
/*******************************************************************************
** Qt Advanced Docking System
** Copyright (C) 2017 Uwe Kindler
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/
```

Must be rewritten as a regular doc-block like this:

```cpp
/*
 * Qt Advanced Docking System
 * Copyright (C) 2017 Uwe Kindler
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; If not, see <http://www.gnu.org/licenses/>.
 */
```

In short remove the superfluous characters which are not needed.

#### Separator Lines

Since modern IDE already provide code navigation, the code does not use separator lines.
All separators must be removed. For example this is removed completely including the line with `INCLUDES` in it.

```cpp
//============================================================================
//                                   INCLUDES
//============================================================================
```
