/*
 * module.c
 *
 *  Created on: 14/10/2017
 *      Author: Miguel
 */

///////////////////////////
///////////////////////////
///////////////////////////
///////////////////////////
/// T
/// O
/// D
/// O
///////////////////////////
///////////////////////////
///////////////////////////
///////////////////////////

#include "module.h"

static module_t * module_list[MODULE_MAX_COUNT];
static int module_count = 0;

static void add_module(module_t * new_module) {
	module_list[module_count++] = new_module;
}

module_t * module_make(size_t module_size, uint32_t address_start, uint32_t address_end) {
	module_t * ret = malloc(module_size + sizeof(module_t));
	ret->address_start = address_start;
	ret->address_end = address_end;
	add_module(ret);
	return ret;
}

module_t * module_get_handle(void * module, size_t module_size) {
	return (module_t*)((int)module + module_size);
}

int module_get_count(void) {
	return module_count;
}
