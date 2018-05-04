/*
 * datapacker.c
 *
 *  Created on: 27/12/2017
 *      Author: Miguel
 */

#include <communications/protocols/packetman/data_packer.h>

int datapacker_get_int(uint8_t * byte_array, int int_idx) {
	int_idx *= 4;
	return (byte_array[int_idx + 3] << 24) | (byte_array[int_idx + 2] << 16) | (byte_array[int_idx + 1] << 8) | byte_array[int_idx];
}
