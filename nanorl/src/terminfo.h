/**
 * @file terminfo.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version pre1.0
 * @date 2024
 * @license LGPLv3.0
 * @brief Terminfo finder and parser.
 */
#pragma once

/**
 * @enum terminfo_entry
 * Internal identifiers for terminfo string entries.
 */
typedef enum {
	TI_CURSOR_LEFT = 0,
	TI_CURSOR_RIGHT,
	TI_KEY_LEFT,
	TI_KEY_RIGHT,
	TI_KEY_BACKSPACE,
	TI_KEY_HOME,
	TI_KEY_END,
	TI_KEYPAD_LOCAL,
	TI_KEYPAD_XMIT,
	TI_ENTRY_COUNT,
} terminfo_entry;

/**
 * @brief Find and parse terminfo file for user's terminal.
 *
 * @return 0 - failed, 1 - success, 2 - partial success.
 */
int nrl_load_terminfo(void);

/**
 * @brief Get printable string for escape sequence.
 *
 * @param[in] name - Interal identifier.
 * @return Printable, null-terminated string.
 * @note Should not be called before nrl_load_terminfo.
 */
const char *nrl_lookup_seq(terminfo_entry name);
