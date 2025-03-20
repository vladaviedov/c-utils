/**
 * @file dfa.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.2.2
 * @date 2024
 * @license LGPLv3.0
 * @brief Simplified DFA for escape sequences.
 */
#define _POSIX_C_SOURCE 200809L
#include "dfa.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "terminfo.h"

typedef struct dfa_node dfa_node;

/**
 * @union dfa_value
 * For edge nodes - children array.
 * For acceptor nodes - accept value.
 */
typedef union {
	dfa_node *children;
	terminfo_input accept;
} dfa_value;

/**
 * @struct dfa_node
 * Node of the DFA tree.
 */
struct dfa_node {
	char edge;
	dfa_value value;
	uint32_t n_children;
};

// Root node; edge doesn't matter
static dfa_node root = {
	.edge = '\0',
	.value.children = NULL,
	.n_children = 0,
};

static void dfa_insert(const char *sequence, terminfo_input accept_value);

void nrl_dfa_build(void) {
	for (uint32_t i = 0; i < TII_COUNT; i++) {
		dfa_insert(nrl_lookup_input(i), i);
	}
}

int nrl_dfa_search(int (*nextch)(), terminfo_input *action) {
	dfa_node *current = &root;

	// Tree is empty
	if (current->n_children == 0) {
		return 0;
	}

	int input;
	while ((input = nextch()) != EOF && input != '\0') {
		// Try to go to a child
		for (uint32_t i = 0; i < current->n_children; i++) {
			dfa_node *child = current->value.children + i;
			if (child->edge == input) {
				current = child;

				// Reached acceptor state
				if (current->n_children == 0) {
					*action = current->value.accept;
					return 1;
				}

				// Have not
				goto search_continue;
			}
		}

		return 0;

search_continue:
		continue;
	}

	return input == EOF ? -1 : 0;
}

/**
 * @brief Insert new sequence into the DFA tree.
 *
 * @param[in] sequence - Sequence to add.
 * @param[in] accept_value - Output value on completed match.
 */
static void dfa_insert(const char *sequence, terminfo_input accept_value) {
	if (sequence == NULL) {
		return;
	}

	dfa_node *current = &root;

	char edge;
	while ((edge = *sequence++) != '\0') {
		for (uint32_t i = 0; i < current->n_children; i++) {
			dfa_node *child = current->value.children + i;
			if (child->edge == edge) {
				current = child;
				goto next;
			}
		}

		current->value.children = realloc(current->value.children,
			(current->n_children + 1) * sizeof(dfa_node));
		dfa_node *child = current->value.children + current->n_children;
		current->n_children++;

		child->edge = edge;
		child->value.children = NULL;
		child->n_children = 0;
		current = child;

next:
		continue;
	}

	current->value.accept = accept_value;
}
