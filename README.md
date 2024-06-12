# c-utils

Collection of libraries providing utils for the C programming language.

## Dependencies

- Build: C tool chain, Make
- Testing: `gtest`, C++ tool chain
- Coverage: `lcov`
- Documentation: `doxygen`

## Build

Edit `build.conf` to select which components of the library should be built and
`make`.

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
