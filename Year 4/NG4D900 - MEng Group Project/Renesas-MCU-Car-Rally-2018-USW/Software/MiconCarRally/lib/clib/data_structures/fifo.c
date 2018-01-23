/*
 * fifo.c
 *
 *  Created on: 15/10/2017
 *      Author: Miguel
 */

#include <stdlib.h>
#include <string.h>
#include "fifo.h"

fifo_t * fifo_create(uint32_t buffer_size) {
	fifo_t * new_fifo       = (fifo_t*)malloc(sizeof(fifo_t));
	new_fifo->buff          = malloc(buffer_size);
	new_fifo->buff16        = 0;
	new_fifo->buff32        = 0;
	new_fifo->buff_fifo_idx = 0;
	new_fifo->buff_fifo_sz  = 0;
	new_fifo->bufflen       = buffer_size;
	return new_fifo;
}

fifo_t * fifo16_create(uint32_t buffer_size) {
	fifo_t * new_fifo       = (fifo_t*)malloc(sizeof(fifo_t));
	new_fifo->buff          = 0;
	new_fifo->buff16        = (uint16_t*)malloc(sizeof(uint16_t) * buffer_size);
	new_fifo->buff32        = 0;
	new_fifo->buff_fifo_idx = 0;
	new_fifo->buff_fifo_sz  = 0;
	new_fifo->bufflen       = buffer_size;
	return new_fifo;
}

fifo_t * fifo32_create(uint32_t buffer_size) {
	fifo_t * new_fifo       = (fifo_t*)malloc(sizeof(fifo_t));
	new_fifo->buff          = 0;
	new_fifo->buff16        = 0;
	new_fifo->buff32        = (uint32_t*)malloc(sizeof(uint32_t) * buffer_size);
	new_fifo->buff_fifo_idx = 0;
	new_fifo->buff_fifo_sz  = 0;
	new_fifo->bufflen       = buffer_size;
	return new_fifo;
}

enum FIFO_ERRCODE fifo_push(fifo_t * handle, uint8_t * buff, uint32_t bufflen) {
	if(!handle)       return FIFO_ERR_INVAL_HANDLE;
	if(!handle->buff) return FIFO_ERR_INVAL_SZ;

	uint32_t newsize = handle->buff_fifo_sz + bufflen;
	if(newsize <= handle->bufflen && buff) {
		memcpy(&handle->buff[handle->buff_fifo_sz], buff, bufflen);
		handle->buff_fifo_sz += bufflen;
		return FIFO_OK;
	} else {
		return FIFO_ERR_FULL;
	}
	return FIFO_OK;
}

enum FIFO_ERRCODE fifo16_push(fifo_t * handle, uint16_t * buff, uint32_t bufflen) {
	if(!handle)         return FIFO_ERR_INVAL_HANDLE;
	if(!handle->buff16) return FIFO_ERR_INVAL_SZ;

	uint32_t newsize = handle->buff_fifo_sz + bufflen;
	if(newsize <= handle->bufflen && buff) {
		memcpy(&handle->buff16[handle->buff_fifo_sz], buff, bufflen);
		handle->buff_fifo_sz += bufflen;
		return FIFO_OK;
	} else {
		return FIFO_ERR_FULL;
	}
	return FIFO_OK;
}

enum FIFO_ERRCODE fifo32_push(fifo_t * handle, uint32_t * buff, uint32_t bufflen) {
	if(!handle)         return FIFO_ERR_INVAL_HANDLE;
	if(!handle->buff32) return FIFO_ERR_INVAL_SZ;

	uint32_t newsize = handle->buff_fifo_sz + bufflen;
	if(newsize <= handle->bufflen && buff) {
		memcpy(&handle->buff32[handle->buff_fifo_sz], buff, bufflen);
		handle->buff_fifo_sz += bufflen;
		return FIFO_OK;
	} else {
		return FIFO_ERR_FULL;
	}
	return FIFO_OK;
}

enum FIFO_ERRCODE fifo_push_byte(fifo_t * handle, uint8_t byte) {
	if(!handle)       return FIFO_ERR_INVAL_HANDLE;
	if(!handle->buff) return FIFO_ERR_INVAL_SZ;

	if(handle->buff_fifo_sz + 1 <= handle->bufflen) {
		handle->buff[handle->buff_fifo_sz] = byte;
		handle->buff_fifo_sz++;
		return FIFO_OK;
	} else {
		return FIFO_ERR_FULL;
	}
}

enum FIFO_ERRCODE fifo_push_short(fifo_t * handle, uint16_t short_val) {
	if(!handle)         return FIFO_ERR_INVAL_HANDLE;
	if(!handle->buff16) return FIFO_ERR_INVAL_SZ;

	if(handle->buff_fifo_sz + 1 <= handle->bufflen) {
		handle->buff16[handle->buff_fifo_sz] = short_val;
		handle->buff_fifo_sz++;
		return FIFO_OK;
	} else {
		return FIFO_ERR_FULL;
	}
}

enum FIFO_ERRCODE fifo_push_word(fifo_t * handle, uint32_t word) {
	if(!handle)         return FIFO_ERR_INVAL_HANDLE;
	if(!handle->buff32) return FIFO_ERR_INVAL_SZ;

	if(handle->buff_fifo_sz + 1 <= handle->bufflen) {
		handle->buff32[handle->buff_fifo_sz] = word;
		handle->buff_fifo_sz++;
		return FIFO_OK;
	} else {
		return FIFO_ERR_FULL;
	}
}

enum FIFO_ERRCODE fifo_flush(fifo_t * handle) {
	if(!handle) return FIFO_ERR_INVAL_HANDLE;

	handle->buff_fifo_sz  = 0;
	handle->buff_fifo_idx = 0;

	if(handle->buff)
		memset(handle->buff,   0, handle->bufflen);
	else if(handle->buff16)
		memset(handle->buff16, 0, handle->bufflen);
	else if(handle->buff32)
		memset(handle->buff32, 0, handle->bufflen);
	else
		return FIFO_ERR_INVAL_BUFFER;

	return FIFO_OK;
}

bool fifo_is_full(fifo_t * handle) {
	if(!handle) return FIFO_ERR_INVAL_HANDLE;

	return handle->buff_fifo_sz == handle->bufflen;
}
