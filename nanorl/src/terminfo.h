#pragma once

typedef enum {
	TI_KEY_LEFT = 0,
	TI_KEY_RIGHT,
	TI_KEY_BACKSPACE,
	TI_KEYPAD_LOCAL,
	TI_KEYPAD_XMIT,
	TI_ENTRY_COUNT,
} terminfo_entry;

int nrl_load_terminfo(void);

const char *nrl_lookup_seq(terminfo_entry name);
