/*
 * fifo.h
 *
 *  Created on: 15/10/2017
 *      Author: Miguel
 */

#ifndef LIB_CLIB_DATA_STRUCTURES_FIFO_H_
#define LIB_CLIB_DATA_STRUCTURES_FIFO_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t  * buff;
	uint16_t * buff16;
	uint32_t * buff32;
	uint32_t   buff_fifo_sz;
	uint32_t   buff_fifo_idx;
	uint32_t   bufflen;
} fifo_t;

enum FIFO_ERRCODE {
	FIFO_OK,
	FIFO_ERR_FULL,
	FIFO_ERR_INVAL_HANDLE,
	FIFO_ERR_INVAL_BUFFER,
	FIFO_ERR_INVAL_SZ,
	FIFO_ERR__COUNT /* How many different types of errors there are */
};

fifo_t          * fifo_create(uint32_t buffer_size);
fifo_t          * fifo16_create(uint32_t buffer_size);
fifo_t          * fifo32_create(uint32_t buffer_size);
enum FIFO_ERRCODE fifo_push(fifo_t * handle, uint8_t  * buff, uint32_t bufflen);
enum FIFO_ERRCODE fifo16_push(fifo_t * handle, uint16_t * buff, uint32_t bufflen);
enum FIFO_ERRCODE fifo32_push(fifo_t * handle, uint32_t * buff, uint32_t bufflen);
enum FIFO_ERRCODE fifo_push_byte(fifo_t * handle, uint8_t byte);
enum FIFO_ERRCODE fifo_push_short(fifo_t * handle, uint16_t short_val);
enum FIFO_ERRCODE fifo_push_word(fifo_t * handle, uint32_t word);
enum FIFO_ERRCODE fifo_flush(fifo_t * handle);
bool              fifo_is_full(fifo_t * handle);

#endif /* LIB_CLIB_DATA_STRUCTURES_FIFO_H_ */
