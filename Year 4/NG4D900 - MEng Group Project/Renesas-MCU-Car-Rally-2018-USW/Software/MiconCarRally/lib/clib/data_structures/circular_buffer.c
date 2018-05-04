/*
 * circular_buffer.c
 *
 *  Created on: 15/10/2017
 *      Author: Miguel
 */

#include <stdlib.h>
#include <string.h>
#include "circular_buffer.h"

void overflow_cursor(circular_t * handle) {
	if(circular_is_full(handle))
		handle->buff_cursor = 0;
}

circular_t * circular_create(uint32_t buffer_size) {
	circular_t * new_circular = (circular_t*)malloc(sizeof(circular_t));
	new_circular->buff        = malloc(buffer_size);
	new_circular->buff_float  = 0;
	new_circular->buff_cursor = 0;
	new_circular->bufflen     = buffer_size;
	return new_circular;
}

circular_t * circular_float_create(uint32_t buffer_size) {
	circular_t * new_circular = (circular_t*)malloc(sizeof(circular_t));
	new_circular->buff        = 0;
	new_circular->buff_float  = (float*)malloc(sizeof(float) * buffer_size);
	new_circular->buff_cursor = 0;
	new_circular->bufflen     = buffer_size;
	return new_circular;
}

enum CIRCULAR_ERRCODE circular_push(circular_t * handle, uint8_t * buff, uint32_t bufflen) {
	if(!handle)       return CIRCULAR_ERR_INVAL_HANDLE;
	if(!handle->buff) return CIRCULAR_ERR_INVAL_SZ;

	uint32_t capped_bufflen = (bufflen > handle->bufflen) ? handle->bufflen : bufflen;
	memcpy(&handle->buff[handle->buff_cursor], buff, capped_bufflen);
	handle->buff_cursor += capped_bufflen;
	overflow_cursor(handle);

	return CIRCULAR_OK;
}

enum CIRCULAR_ERRCODE circular_push_float_buffer(circular_t * handle, float * buff, uint32_t bufflen) {
	if(!handle)             return CIRCULAR_ERR_INVAL_HANDLE;
	if(!handle->buff_float) return CIRCULAR_ERR_INVAL_SZ;

	uint32_t capped_bufflen = (bufflen > handle->bufflen) ? handle->bufflen : bufflen;
	memcpy(&handle->buff_float[handle->buff_cursor], buff, capped_bufflen);
	handle->buff_cursor += capped_bufflen;
	overflow_cursor(handle);

	return CIRCULAR_OK;
}

enum CIRCULAR_ERRCODE circular_push_byte(circular_t * handle, uint8_t byte) {
	if(!handle)             return CIRCULAR_ERR_INVAL_HANDLE;
	if(!handle->buff_float) return CIRCULAR_ERR_INVAL_SZ;

	handle->buff[handle->buff_cursor++] = byte;
	overflow_cursor(handle);

	return CIRCULAR_OK;
}

enum CIRCULAR_ERRCODE circular_push_float(circular_t * handle, float value) {
	if(!handle)             return CIRCULAR_ERR_INVAL_HANDLE;
	if(!handle->buff_float) return CIRCULAR_ERR_INVAL_SZ;

	handle->buff_float[handle->buff_cursor++] = value;
	overflow_cursor(handle);

	return CIRCULAR_OK;
}

enum CIRCULAR_ERRCODE circular_flush(circular_t * handle) {
	if(!handle) return CIRCULAR_ERR_INVAL_HANDLE;

	handle->buff_cursor  = 0;
	if(handle->buff)
		memset(handle->buff, 0, handle->bufflen);
	else if(handle->buff_float)
		memset(handle->buff_float, 0, handle->bufflen);
	else
		return CIRCULAR_ERR_INVAL_BUFFER;

	return CIRCULAR_OK;
}

bool circular_is_full(circular_t * handle) {
	if(!handle) return CIRCULAR_ERR_INVAL_HANDLE;

	return handle->buff_cursor >= handle->bufflen;
}
