#include "terminfo.h"

#include <stddef.h>

// TODO: remove
// Standard reference codes
static const char kcub1[] = { 0x1b, 0x5b, 0x44, 0x00 };
static const char kcuf1[] = { 0x1b, 0x5b, 0x43, 0x00 };
static const char kbs[] = { 0x7f, 0x00 };

// TODO: replace placeholder
const char *nrl_lookup_seq(nrl_terminfo_entry name) {
	switch (name) {
	case NRL_TI_LEFT:
		return kcub1;
	case NRL_TI_RIGHT:
		return kcuf1;
	case NRL_TI_BACKSPACE:
		return kbs;
	}

	return NULL;
}
