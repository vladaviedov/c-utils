/**
 * @file fastload.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.0
 * @date 2024
 * @license LGPLv3.0
 * @brief Terminfo stubs for common terminals.
 */
#define _POSIX_C_SOURCE 200809L
#include "fastload.h"

#if ENABLE_FASTLOAD == 1
#include <string.h>

#include "terminfo.h"

static const char *xterm_stub[TI_ENTRY_COUNT] = {
	"\b",
	"\033[C",
	"\033[?1h\033>",
	"\033[?1h\033=",
	"\033OD",
	"\033OC",
	"\177",
	"\033OH",
	"\033OF",
};

void nrl_fl_xterm(const char **cache) {
	memcpy(cache, &xterm_stub, TI_ENTRY_COUNT * sizeof(char *));
}
#endif
