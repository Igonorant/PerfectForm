# Perfect Form

It is all about a jelly alien form with a single purpose: absorb all the genome of this planet and become the PERFECT FORM!

## Compiling

We recommend installing [MSYS2](https://www.msys2.org/)
Open the `MSYS2 UCRT64` prompt and then ensure you've installed the following packages. This will get you working toolchain, CMake, Ninja, and of course SDL3.

```sh
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-sdl3
```

Compile using:
```sh
cmake -S . -B build
cmake --build build
```
