# nanorl

## Description

`nanorl` (nano readline) is a small line editing library. Unlike `readline`
(GPL) or `libedit` (BSD), `nanorl` does not provide history, completions or
editor-specific options.

`nanorl` implements the following interactions:
- Standard text input
- Backspace
- Arrow navigation
- Home & End keys
- Non-interactive mode (e.g. reading from a pipe)

`nanorl` is written for POSIX, but relies on the curses terminfo database,
although not the library itself.

## Configuration

List of configuration macros located in `config.h`.

|Macro|Description|Default|
|---|---|---|
|DEBIAN_DIRS|Include Debian-specific terminfo locations|1|
|ENABLE_FASTLOAD|Builtin terminfo data for popular terminals (xterm)|1|

## Changelog

- 1.1
```
Rework of input acquisition
Unused escape sequences are printed in a readable format
Improved support for non-tty input

Fixed: Pasted text is now processed
```
- 1.0
```
First release
```
