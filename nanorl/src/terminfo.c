/**
 * @file terminfo.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.2.2
 * @date 2024
 * @license LGPLv3.0
 * @brief Terminfo finder and parser.
 */
#define _POSIX_C_SOURCE 200809L
#include "terminfo.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "fastload.h"

// Reference: man term
#define MAGIC_INT16 0432
#define MAGIC_INT32 01036

// Reference: man terminfo
static const char *db_path[] = {
	// Most common location used by:
	// Archlinux, OpenBSD, DragonFlyBSD, macOS
	"/usr/share/terminfo",
#if DEBIAN_DIRS == 1
	"/etc/terminfo",
	"/lib/terminfo",
#endif
#if FREEBSD_DIRS == 1
	"/usr/share/etc/terminfo",
#endif
#if NETBSD_DIRS == 1
	"/usr/share/misc/terminfo",
#endif
	NULL,
};

// Reference: ncurses <term.h> lists these
// It would be nice to have a better reference
static const uint8_t input_offsets[] = {
	79u,  // key_left
	83u,  // key_right
	55u,  // key_backspace
	76u,  // key_home
	164u, // key_end
	59u,  // key_dc (delete)
};
static const uint8_t output_offsets[] = {
	14u, // cursor_left
	17u, // cursor_right
	88u, // keypad_local
	89u, // keypad_xmit
};

static int is_loaded = 0;
static char *strings_table = NULL;
static const char *inputs[TII_COUNT] = { NULL };
static const char *outputs[TIO_COUNT] = { NULL };

static FILE *find_terminfo(const char *term);
static FILE *try_open(const char *ti_path, const char *term);
static int parse(FILE *terminfo);

int nrl_load_terminfo(void) {
	// Already loaded
	if (is_loaded) {
		return 1;
	}

	const char *term = getenv("TERM");
	if (term == NULL) {
		return 0;
	}

#if ENABLE_FASTLOAD == 1
	// Fastload xterm-based terminals
	if (strstr(term, "xterm") != NULL) {
		nrl_fl_xterm(inputs, outputs);
		is_loaded = 1;
		return 1;
	}
#endif

	// Do a proper search
	FILE *terminfo = find_terminfo(term);
	if (terminfo == NULL) {
		return 0;
	}

	return parse(terminfo);
}

const char *nrl_lookup_input(terminfo_input name) {
	return inputs[name];
}

const char *nrl_lookup_output(terminfo_output name) {
	return outputs[name];
}

static FILE *find_terminfo(const char *term) {
	// Search for the terminfo file
	FILE *terminfo = NULL;

	// Check $TERMINFO
	const char *ti_env = getenv("TERMINFO");
	if (ti_env != NULL) {
		terminfo = try_open(getenv("TERMINFO"), term);

		if (terminfo != NULL) {
			return terminfo;
		}
	}

	// Local user terminfo
	const char *home = getenv("HOME");
	if (home != NULL) {
		const char *from_home = "/.terminfo";
		uint32_t ti_local_len = strlen(home) + strlen(from_home) + 1;
		char ti_local[ti_local_len];
		snprintf(ti_local, ti_local_len, "%s%s", home, from_home);

		terminfo = try_open(ti_local, term);
		if (terminfo != NULL) {
			return terminfo;
		}
	}

	// Check $TERMINFO_DIRS
	char *dirs = getenv("TERMINFO_DIRS");
	if (dirs != NULL) {
		char *dirs_copy = strdup(dirs);

		char *dir = strtok(dirs_copy, ":");
		while (dir != NULL) {
			terminfo = try_open(dir, term);
			if (terminfo != NULL) {
				free(dirs_copy);
				return terminfo;
			}

			dir = strtok(NULL, ":");
		}

		free(dirs_copy);
	}

	// Other common locations
	const char **db_trav = db_path;
	const char *db;
	while ((db = *db_trav++) != NULL) {
		terminfo = try_open(db, term);
		if (terminfo != NULL) {
			return terminfo;
		}
	}

	return NULL;
}

/**
 * @brief Try a certain terminfo database directory.
 *
 * @param[in] ti_path - Path to database.
 * @param[in] term - Name of terminal.
 * @return Open file or NULL if not found.
 */
static FILE *try_open(const char *ti_path, const char *term) {
	// Path format: TERMINFO/INITIAL/TERMINAL \0
	uint32_t length = strlen(ti_path) + 3 + strlen(term) + 1;
	char full_path[length];
	snprintf(full_path, length, "%s/%c/%s", ti_path, term[0], term);

	return fopen(full_path, "r");
}

/**
 * @brief Parse terminfo file.
 *
 * @param[in] terminfo - Input file.
 * @return 0 - file invalid, 1 - good, 2 - incomplete.
 */
static int parse(FILE *terminfo) {
	// Reference: man term
	uint16_t header[6];
	fread(header, sizeof(uint16_t), 6, terminfo);

	int numbers_element_size = 0;
	switch (header[0]) {
	case MAGIC_INT16:
		numbers_element_size = sizeof(int16_t);
		break;
	case MAGIC_INT32:
		numbers_element_size = sizeof(int32_t);
		break;
	default:
		return 0;
	}

	uint32_t str_start = header[1] + header[2];
	// Extra padding byte
	if (str_start & 1) {
		str_start++;
	}
	str_start += numbers_element_size * header[3];

	// Load strings section
	fseek(terminfo, str_start, SEEK_CUR);
	int16_t strings[header[4]];
	fread(strings, sizeof(int16_t), header[4], terminfo);
	// Load strings table
	strings_table = malloc(header[5]);
	fread(strings_table, 1, header[5], terminfo);

	// Look up all input capabilities needed
	int ret_code = 1;
	for (uint32_t i = 0; i < TII_COUNT; i++) {
		uint8_t index = input_offsets[i];
		int16_t offset = strings[index];
		if (offset == -1) {
			ret_code = 2;
		}

		char *value = strings_table + offset;
		if (value == NULL || strlen(value) == 0) {
			inputs[i] = NULL;
		} else {
			inputs[i] = value;
		}
	}
	for (uint32_t i = 0; i < TIO_COUNT; i++) {
		uint8_t index = output_offsets[i];
		int16_t offset = strings[index];
		if (offset == -1) {
			ret_code = 2;
		}

		char *value = strings_table + offset;
		if (value == NULL || strlen(value) == 0) {
			outputs[i] = NULL;
		} else {
			outputs[i] = value;
		}
	}

	fclose(terminfo);
	is_loaded = 1;

	return ret_code;
}
