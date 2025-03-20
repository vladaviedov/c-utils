/**
 * @file config.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.2.2
 * @date 2024
 * @license LGPLv3.0
 * @brief Compile-time configuration.
 */
#pragma once

// Enable Debian-specific terminfo locations
#define DEBIAN_DIRS 1
// Enable FreeBSD-specific  terminfo locations
#define FREEBSD_DIRS 1
// Enable NetBSD-specific terminfo locations
#define NETBSD_DIRS 1
// Enable pre-compiled terminfo stubs
#define ENABLE_FASTLOAD 1
