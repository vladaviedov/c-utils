# nanorl

## Description

`nanorl` (nano readline) is a small line editing library. Unlike `readline`
(GPL) or `libedit` (BSD), `nanorl` does not provide history, completions or
editor-specific options.

`nanorl` implements the following interactions:
- Standard text input
- Backspace
- Arrow navigation
- Home, End, Delete keys
- Input from a pipe
- Input obfuscation (option)

`nanorl` is written for POSIX 2008 and relies on the curses terminfo database,
although not the library itself.

## Configuration

List of configuration macros located in `config.h`.

|Macro|Description|Default|
|---|---|---|
|DEBIAN_DIRS|Include Debian-specific terminfo locations|1|
|FREEBSD_DIRS|Include FreeBSD-specific terminfo locations|1|
|NETBSD_DIRS|Include NetBSD-specific terminfo locations|1|
|ENABLE_FASTLOAD|Built-in terminfo data for popular terminals (xterm)|1|

## Changelog

- 1.2
```
Support deleting with the 'Delete' key
More database paths (FreeBSD, NetBSD)

Fixed: DEBIAN_DIRS is now used in the code instead of DEBIAN
```
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
