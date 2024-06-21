/**
 * @file terminfo.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.1
 * @date 2024
 * @license LGPLv3.0
 * @brief Terminfo finder and parser.
 */
#pragma once

/**
 * @enum terminfo_output
 * Internal identifiers for terminfo input sequences.
 */
typedef enum {
	TII_KEY_LEFT,
	TII_KEY_RIGHT,
	TII_KEY_BACKSPACE,
	TII_KEY_HOME,
	TII_KEY_END,
	TII_KEY_INSERT,
	TII_KEY_DELETE,
} terminfo_input;
#define TII_COUNT 7

/**
 * @enum terminfo_output
 * Internal identifiers for terminfo output sequences.
 */
typedef enum {
	TIO_CURSOR_LEFT,
	TIO_CURSOR_RIGHT,
	TIO_KEYPAD_LOCAL,
	TIO_KEYPAD_XMIT,
} terminfo_output;
#define TIO_COUNT 4

/**
 * @brief Find and parse terminfo file for user's terminal.
 *
 * @return 0 - failed, 1 - success, 2 - partial success.
 */
int nrl_load_terminfo(void);

/**
 * @brief Get printable string for input escape sequence.
 *
 * @param[in] name - Interal identifier.
 * @return ASCII representation, null-terminated string.
 * @note Should not be called before nrl_load_terminfo.
 */
const char *nrl_lookup_input(terminfo_input name);

/**
 * @brief Get printable string for output escape sequence.
 *
 * @param[in] name - Interal identifier.
 * @return ASCII representation, null-terminated string.
 * @note Should not be called before nrl_load_terminfo.
 */
const char *nrl_lookup_output(terminfo_output name);
