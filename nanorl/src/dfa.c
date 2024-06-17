/**
 * @file dfa.c
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.1
 * @date 2024
 * @license LGPLv3.0
 * @brief Simplified DFA for escape sequences.
 */
#define _POSIX_C_SOURCE 200809L
#include "dfa.h"

#include <stdlib.h>
#include <stdint.h>

#include "terminfo.h"

typedef struct dfa_node dfa_node;

/**
 * @union dfa_value
 * For edge nodes - children array.
 * For acceptor nodes - accept value.
 */
typedef union {
	dfa_node *children;
	terminfo_entry accept;
} dfa_value;

/**
 * @struct dfa_node
 * Node of the DFA tree.
 * 
 * @var dfa_node::edge
 * Transition value to get to this node.
 *
 * @var dfa_node::value
 * Value stored in the node.
 *
 * @var dfa_node::n_children
 * Children count. Used to distinguish node types.
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

static void dfa_insert(const char *sequence, terminfo_entry accept_value);

void nrl_dfa_build(void) {
	for (terminfo_entry i = TI_KEY_LEFT; i < TI_ENTRY_COUNT; i++) {
		dfa_insert(nrl_lookup_seq(i), i);
	}
}

int nrl_dfa_search(char (*nextch)(), terminfo_entry *action) {
	dfa_node *current = &root;

	// Tree is empty
	if (current->n_children == 0) {
		return 0;
	}
	
	char input;
	while ((input = nextch()) != '\0') {
		if (current->n_children == 0) {
			*action = current->value.accept;
			return 1;
		}

		for (uint32_t i = 0; i < current->n_children; i++) {
			dfa_node *child = current->value.children + i;
			if (child->edge == input) {
				current = child;
				break;
			}
		}
	}

	return 0;
}

static void dfa_insert(const char *sequence, terminfo_entry accept_value) {
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

		current->value.children = realloc(current->value.children, current->n_children + 1);
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
