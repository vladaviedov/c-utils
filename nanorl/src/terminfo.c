/**
 * @file terminfo.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version pre1.0
 * @date 2024
 * @license LGPLv3.0
 * @brief Terminfo finder and parser.
 */
#define _POSIX_C_SOURCE 200809L
#include "terminfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Enable Debian-specific terminfo locations
#define DEBIAN 1

// Reference: man term
#define MAGIC_INT16 0432
#define MAGIC_INT32 01036

// Reference: man terminfo
static const char *db_path[] = {
	"/usr/share/terminfo",
#if DEBIAN == 1
	"/etc/terminfo",
	"/lib/terminfo",
#endif
	NULL,
};

// Reference: ncurses <term.h> lists these
// It would be nice to have a better reference
static const uint8_t offsets[] = {
	14u,  // cursor_left
	17u,  // cursor_right
	79u,  // key_left
	83u,  // key_right
	55u,  // key_backspace
	76u,  // key_home
	164u, // key_end
	88u,  // keypad_local
	89u,  // keypad_xmit
};
static const uint32_t offsets_len = sizeof(offsets) / sizeof(uint8_t);

static char *strings_table = NULL;
static const char *cache[TI_ENTRY_COUNT] = { NULL };

static FILE *find_terminfo(const char *term);
static FILE *try_open(const char *ti_path, const char *term);
static int parse(FILE *terminfo);

int nrl_load_terminfo(void) {
	const char *term = getenv("TERM");
	
	// Check for shortcuts
	if (strstr(term, "xterm") != NULL) {
		// TODO: xterm shortcut
		/* return 1; */
	}

	// Do a proper search
	FILE *terminfo = find_terminfo(term);
	if (terminfo == NULL) {
		return 0;
	}

	return parse(terminfo);
}

const char *nrl_lookup_seq(terminfo_entry name) {
	return cache[name];
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

	// Look up all capabilities needed
	int ret_code = 1;
	for (uint32_t i = 0; i < offsets_len; i++) {
		uint8_t index = offsets[i];
		int16_t offset = strings[index];
		if (offset == -1) {
			ret_code = 2;
		}

		cache[i] = strings_table + offset;
	}
	
	return ret_code;
}
