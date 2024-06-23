/**
 * @file fastload.h
 * @author Vladyslav Aviedov <vladaviedov at protonmail dot com>
 * @version 1.2
 * @date 2024
 * @license LGPLv3.0
 * @brief Terminfo stubs for common terminals.
 */
#pragma once

#include "config.h"

#if ENABLE_FASTLOAD == 1
/**
 * @brief Load 'xterm' configuration.
 *
 * @param[out] inputs - Input storage to fill.
 * @param[out] outputs - Output storage to fill.
 */
void nrl_fl_xterm(const char **inputs, const char **outputs);
#endif
