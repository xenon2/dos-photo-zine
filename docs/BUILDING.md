# Building

Install Open Watcom C and make sure `wcl` is on `PATH`.

Tested compiler:

```text
Open Watcom C/C++ x86 16-bit Compile and Link Utility
Version 2.0 beta Dec  8 2023 19:09:05 (64-bit)
```

Then run from the repository root:

```sh
./build.sh
```

The DOS executable is written to `main.exe`.

To prepare a distributable package:

```sh
./release.sh
```

This builds the program and creates `RELEASE/` plus `dos-photo-zine.zip`.
