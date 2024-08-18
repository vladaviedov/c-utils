/**
 * @file nanorl.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.2.1
 * @date 2024
 * @license LGPLv3.0
 * @brief Small and simple line editing library.
 */
#pragma once

/**
 * Library version.
 */
#define NRL_LIB_VER "1.2.1"

/**
 * @enum nrl_error
 * Optional error code for nanorl functions.
 *
 * @var nrl_error::NRL_ERR_OK
 * No errors were detected.
 *
 * @var nrl_error::NRL_ERR_EMPTY
 * No user input provided.
 *
 * @var nrl_error::NRL_ERR_BAD_FD
 * Library invoked with a bad file descriptor.
 *
 * @var nrl_error::NRL_ERR_SYS
 * Operating system error; check errno for more info.
 */
typedef enum {
	NRL_ERR_OK = 0,
	NRL_ERR_EMPTY = 1,
	NRL_ERR_BAD_FD = 2,
	NRL_ERR_SYS = 3,
} nrl_error;

/**
 * @enum nrl_echo_mode
 * Echo style.
 *
 * @var nrl_echo_mode::NRL_ECHO_NO
 * Disable echo completely.
 *
 * @var nrl_echo_mode::NRL_ECHO_YES
 * Echo user input.
 *
 * @var nrl_echo_mode::NRL_ECHO_FAKE
 * Replace typed characters with a symbol.
 */
typedef enum {
	NRL_ECHO_NO = 0,
	NRL_ECHO_YES = 1,
	NRL_ECHO_FAKE = 2,
} nrl_echo_mode;

/**
 * @struct nrl_opts
 * Nanorl configuration options.
 *
 * @var nrl_opts::fd
 * Terminal file descriptor.
 *
 * @var nrl_opts::prompt
 * Prompt for input.
 *
 * @var nrl_opts::echo
 * Echo style.
 *
 * @var nrl_opts::echo_repl
 * Replacement character (for NRL_FAKE_ECHO).
 */
typedef struct {
	int fd;
	const char *prompt;
	nrl_echo_mode echo;
	char echo_repl;
} nrl_opts;

/**
 * @brief Run nanorl on standard input.
 *
 * @param[in] prompt - User prompt.
 * @param[out] err - Error code buffer (optional).
 * @return User input.
 */
char *nanorl(const char *prompt, nrl_error *err);

/**
 * @brief Run nanorl on another file descriptor.
 *
 * @param[in] fd - File descriptor.
 * @param[in] prompt - User prompt.
 * @param[out] err - Error code buffer (optional).
 * @return User input.
 */
char *nanorl_fd(int fd, const char *prompt, nrl_error *err);

/**
 * @brief Run nanorl with extra options.
 *
 * @param[in] options - Configuration.
 * @param[out] err - Error code buffer (optional).
 * @return User input.
 */
char *nanorl_opts(const nrl_opts *options, nrl_error *err);
