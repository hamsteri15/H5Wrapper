# A small wrapper around the HDF5 parallel C-API


## Overview


Although, the library is intended to be as dependency-free as possible, the following libraries are still required:

- C++17
- [CMake](https://cmake.org/) build script for cross-platform configuration (see 
  [here](#cmakeliststxt) for more info)
- [Open MPI](https://www.open-mpi.org/) for distributed memory parallelization
- [HDF5-parallel](https://www.hdfgroup.org) for output routines

In addition the following libraries are used but they ship alongside the library

- [Catch](https://github.com/catchorg/Catch2) unit testing framework (see 
  [here](#mytestscpp))

The following tools are also used but are not mandatory

- [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) for automated 
  source code formatting (see [here](#clang-format))
- [Clang-Tidy](http://clang.llvm.org/extra/clang-tidy/) for static analysis to 
  catch coding errors (see [here](#clang-tidy))




### Linux

```bash
$ mkdir build
$ cd build
$ cmake ..
```
