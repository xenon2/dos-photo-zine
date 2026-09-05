# Building

Install Open Watcom C and set `WATCOM` to its installation directory. If
`wcl` is not already on `PATH`, `build_dos-zine_bin.sh` selects `binl64` or `binl` on Linux.

Tested compiler:

```text
Open Watcom C/C++ x86 16-bit Compile and Link Utility
Version 2.0 beta Dec  8 2023 19:09:05 (64-bit)
```

Then run from the repository root:

```sh
WATCOM=/path/to/open-watcom ./build_dos-zine_bin.sh
```

The build explicitly uses Watcom's `lib286` libraries because they provide the
16-bit runtime required by the real-mode `-ms` target. The default `-2` option
generates code for an Intel 80286, the minimum supported CPU. `lib386` is for
32-bit targets and is not used by this program.

A successful build replaces the canonical root `ZINE.EXE`. This is the same
file used by local test and packaging scripts, so there is no separate old
binary that might be tested accidentally. `make clean` removes object files
but deliberately preserves the supplied executable. The default build targets
an 80286 or newer CPU and favors runtime speed:

- `-2` allows 286 instructions and selects Watcom's 286 scheduling model.
- `-ox` enables Watcom's full optimization set, including loop, intrinsic,
  branch, and instruction-scheduling optimizations. It also disables generated
  stack-overflow checks.
- `-ot` favors execution speed over code size when the optimizer has a choice.

The program remains a 16-bit real-mode DOS executable. To build for an older
CPU, or to compare another instruction or optimization profile, override the
Make variables. For example:

```sh
make clean
make CPU_FLAGS=-0 OPT_FLAGS=-ol  # 8086-compatible experimental build

# Alternatively, after cleaning again:
make clean
make CPU_FLAGS=-3                # 386-or-newer build
```

Overriding `CFLAGS` directly is also supported. A clean build is required after
changing flags because Make does not track command-line changes as dependencies.

To package the current `ZINE.EXE` and already-converted image data without
invoking Watcom:

```sh
./package.sh
```

Maintainers can run `./release.sh` for the complete source-build, image
conversion, and packaging workflow.
