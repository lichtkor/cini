# cini

> A simple, lightweight, header only `.ini` file parser and writer built for both **C** and **C++** projects.

---

## What is this?

When working on small apps, games, or utilities, you often just need a straightforward way to save and load configuration settings without bringing in massive, complex dependencies. **cini** gives you a clean, zero fuss solution.

It comes in two ways depending on your tech stack:
* **`cini.h`** A traditional, single header C library.
* **`cpp/Cini.hpp`** A modern C++ header only wrapper using standard containers (`std::unordered_map`), type safe templates, and `std::string_view`.

---

## Getting Started

### For C++ (`Cini.hpp`)
Drop `Cini.hpp` into your project and start parsing right away
by including `#include "Cini.hpp"`

### For C (`cini.h`)
In one C source file, define the implementation macro before including
`#define CINI_IMPLEMENTATION`
`#include "cini.h"`

### example code
Check out the [C code example](./main.c) to see how it's used.
Check out the [C++ code example](./cpp/main.cpp) to see how it's used.
