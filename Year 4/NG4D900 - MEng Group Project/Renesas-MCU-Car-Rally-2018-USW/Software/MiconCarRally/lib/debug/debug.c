/*
 * debug.c
 *
 *  Created on: 01/11/2017
 *      Author: Miguel
 */

#include <stddef.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <utils.h>
#include <communications/protocols/packetman/packetman.h>
#include "debug.h"

bool is_debugging = false;

void debug_set(bool enable_debugging) {
	is_debugging = enable_debugging;
}
