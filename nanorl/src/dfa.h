/**
 * @file dfa.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.2.2
 * @date 2024
 * @license LGPLv3.0
 * @brief Simplified DFA for escape sequences.
 */
#pragma once

#include "terminfo.h"

/**
 * @brief Build a tree from terminfo data.
 */
void nrl_dfa_build(void);

/**
 * @brief Search tree for an escape sequence.
 *
 * @param[in] nextch - Function to retrieve next character.
 * @param[out] action - Buffer for escape sequence type.
 * @return Whether action was found. -1 on EOF received.
 */
int nrl_dfa_search(int (*nextch)(), terminfo_input *action);
