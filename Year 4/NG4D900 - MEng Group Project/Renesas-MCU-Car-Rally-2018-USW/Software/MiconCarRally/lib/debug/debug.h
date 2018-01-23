/*
 * debug.h
 *
 *  Created on: 01/11/2017
 *      Author: Miguel
 */

#ifndef LIB_DEBUG_DEBUG_H_
#define LIB_DEBUG_DEBUG_H_

#include <stdbool.h>
#include <utils.h>

#define DEBUG_PROMPT "DEBUG>"

extern bool is_debugging;

void debug_set(bool enable_debugging);

#define DEBUG_HELPER(fmt, ...) if(is_debugging) { printf("\n%s ", DEBUG_PROMPT); printf(fmt, __VA_ARGS__); }
#define DEBUG(...) DEBUG_HELPER(__VA_ARGS__, 0)

#endif /* LIB_DEBUG_DEBUG_H_ */
