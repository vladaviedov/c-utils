# c-utils

Collection of utility libraries for the C programming language.

## Dependencies

- Build: C tool chain, GNU Make, AWK
- Testing: `gtest`, C++ tool chain
- Coverage: `lcov`
- Documentation: `doxygen`

## Targets

The code and build system are written for Unix(-like) operating systems. Some
libraries might work under different OSes or require special considerations.

All code is C99, but some libraries might use POSIX or other extensions (see
inner README files).

My goal is for the code to work on:
- Linux (Debian, Arch)
- BSD (FreeBSD)
- macOS

## Build

Edit `build.conf` to select which components of the library should be built and
`make`.

### Older Versions

Switch to older versions of libraries (for building) using the `version.sh`
script.
```
./version.sh <library> <version|"latest">
```

Reset back to latest with (instead of `git reset`):
```
./version.sh reset
```

For special notes about this see [here](./VERSION.md)

### Output

- Static library - `build/lib/libutils.a`
- Headers - `build/include`

## Testing

All `build.conf` settings should be enabled to run all tests. Run tests with
`make test`.

### Coverage

Coverage information is generated with `make coverage`. The output html is
generated in `cov/`.

## Documentation

Documentation can be generated with `make docs`. Output is written to `docs/`.
