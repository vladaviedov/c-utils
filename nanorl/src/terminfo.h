#pragma once

typedef enum {
	NRL_TI_LEFT,
	NRL_TI_RIGHT,
	NRL_TI_BACKSPACE,
} nrl_terminfo_entry;

const char *nrl_lookup_seq(nrl_terminfo_entry name);
