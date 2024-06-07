#pragma once

typedef enum {
	NRL_TI_LEFT = 0,
	NRL_TI_RIGHT,
	NRL_TI_BACKSPACE,
	NRL_ENTRY_COUNT,
} nrl_terminfo_entry;

int nrl_load_terminfo(void);

const char *nrl_lookup_seq(nrl_terminfo_entry name);
