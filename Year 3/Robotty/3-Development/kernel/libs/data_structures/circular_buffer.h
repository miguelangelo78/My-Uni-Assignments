/*
 * circular_buffer.h
 *
 *  Created on: 03/03/2017
 *      Author: Miguel
 */

#ifndef LIBS_DATA_STRUCTURES_CIRCULAR_BUFFER_H_
#define LIBS_DATA_STRUCTURES_CIRCULAR_BUFFER_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct {
	uint8_t * buff;
	float   * buff_float;
	uint32_t  bufflen;
	uint32_t  buff_cursor;
} circular_t;

enum CIRCULAR_ERRCODE {
	CIRCULAR_OK,
	CIRCULAR_ERR_INVAL_HANDLE,
	CIRCULAR_ERR_INVAL_BUFFER,
	CIRCULAR_ERR_INVAL_SZ,
	CIRCULAR_ERR__COUNT /* How many different types of errors there are */
};

circular_t          * circular_create(uint32_t buffer_size);
circular_t          * circular_float_create(uint32_t buffer_size);
enum CIRCULAR_ERRCODE circular_push(circular_t * handle, uint8_t  * buff, uint32_t bufflen);
enum CIRCULAR_ERRCODE circular_push_float_buffer(circular_t * handle, float * buff, uint32_t bufflen);
enum CIRCULAR_ERRCODE circular_push_byte(circular_t * handle, uint8_t byte);
enum CIRCULAR_ERRCODE circular_push_float(circular_t * handle, float value);
enum CIRCULAR_ERRCODE circular_flush(circular_t * handle);
bool                  circular_is_full(circular_t * handle);

#endif /* LIBS_DATA_STRUCTURES_CIRCULAR_BUFFER_H_ */
