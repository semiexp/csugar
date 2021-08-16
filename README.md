# csugar
Reimplementation of Sugar CSP solver in C++

## How to build

[CMake](https://cmake.org/) is required to build csugar.

```
$ git clone https://github.com/semiexp/csugar.git
$ cd csugar
$ mkdir build
$ cd build

$ cmake -DCMAKE_BUILD_TYPE=Release .. && make
# To use Glucose backend, use the following command instead:
$ cmake -DCMAKE_BUILD_TYPE=Release -DUSE_GLUCOSE=1 .. && make

# Optional: installing Python binding
$ pip install .
```
