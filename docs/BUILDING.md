# Building

Install Open Watcom C and set `WATCOM` to its installation directory. If
`wcl` is not already on `PATH`, `build.sh` selects `binl64` or `binl` on Linux.

Tested compiler:

```text
Open Watcom C/C++ x86 16-bit Compile and Link Utility
Version 2.0 beta Dec  8 2023 19:09:05 (64-bit)
```

Then run from the repository root:

```sh
WATCOM=/path/to/open-watcom ./build.sh
```

The build explicitly uses Watcom's `lib286` libraries because they provide the
16-bit runtime required by the real-mode `-mh` target. The `-3` option may still
generate 386 instructions suitable for a 386SX; `lib386` is for 32-bit targets.

The DOS executable is written to `main.exe`. The default build targets an
80386 or newer CPU and favors runtime speed:

- `-3` allows 386 instructions and selects Watcom's 386 scheduling model.
- `-ox` enables Watcom's full optimization set, including loop, intrinsic,
  branch, and instruction-scheduling optimizations. It also disables generated
  stack-overflow checks.
- `-ot` favors execution speed over code size when the optimizer has a choice.

The program remains a 16-bit real-mode DOS executable; `-3` changes generated
instructions, not the memory model. To build for an older CPU or compare a less
aggressive optimization profile, override the Make variables, for example:

```sh
make clean
make CPU_FLAGS=-0 OPT_FLAGS=-ol
```

Overriding `CFLAGS` directly is also supported. A clean build is required after
changing flags because Make does not track command-line changes as dependencies.

To prepare a distributable package:

```sh
./release.sh
```

This builds the program and creates `RELEASE/` plus `dos-photo-zine.zip`.
