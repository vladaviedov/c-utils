#define _POSIX_C_SOURCE 200809L
#include "terminfo.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

static const char *db_path[] = {
	"/usr/share/terminfo",
	"/lib/terminfo",
	NULL,
};
static const char *cache[NRL_ENTRY_COUNT] = { NULL };

static FILE *find_terminfo(const char *term);
static FILE *try_open(const char *ti_path, const char *term);
static int parse(FILE *terminfo);

// TODO: remove
// Standard reference codes
/* static const char kcub1[] = { 0x1b, 0x5b, 0x44, 0x00 }; */
/* static const char kcuf1[] = { 0x1b, 0x5b, 0x43, 0x00 }; */
/* static const char kbs[] = { 0x7f, 0x00 }; */

int nrl_load_terminfo(void) {
	const char *term = getenv("TERM");
	
	// Check for shortcuts
	if (strstr(term, "xterm") != NULL) {
		// TODO: xterm shortcut
		return 1;
	}

	// Do a proper search
	FILE *terminfo = find_terminfo(term);
	if (terminfo == NULL) {
		return 0;
	}

	// TODO: Parse terminfo

	return 1;
}

const char *nrl_lookup_seq(nrl_terminfo_entry name) {
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

static FILE *try_open(const char *ti_path, const char *term) {
	// Path format: TERMINFO/INITIAL/TERMINAL \0
	uint32_t length = strlen(ti_path) + 3 + strlen(term) + 1;
	char full_path[length];
	snprintf(full_path, length, "%s/%c/%s", ti_path, term[0], term);

	return fopen(full_path, "r");
}

static int parse(FILE *terminfo) {
	return 1;
}
