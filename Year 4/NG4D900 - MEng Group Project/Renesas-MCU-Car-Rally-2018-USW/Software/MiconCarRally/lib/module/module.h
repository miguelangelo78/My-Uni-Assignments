/*
 * module.h
 *
 *  Created on: 14/10/2017
 *      Author: Miguel
 */

#ifndef LIB_MODULE_MODULE_H_
#define LIB_MODULE_MODULE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#define MODULE_MAX_COUNT 32

typedef void * (*module_fp_t)(void*);

typedef struct {
	module_fp_t open;
	module_fp_t ctrl;
	module_fp_t close;
	uint32_t    address_start;
	uint32_t    address_end;
} module_t;

typedef enum ret {
	RET_NULL =  0,
	RET_MAX  = -1
} ret_t;

module_t * module_make(size_t module_size, uint32_t address_start, uint32_t address_end);
module_t * module_get_handle(void * module, size_t module_size);
int        module_get_count(void);

#endif /* LIB_MODULE_MODULE_H_ */
