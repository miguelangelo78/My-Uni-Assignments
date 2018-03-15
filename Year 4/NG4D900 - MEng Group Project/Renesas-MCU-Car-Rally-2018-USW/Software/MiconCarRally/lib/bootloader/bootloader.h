/*
 * bootloader.h
 *
 *  Created on: 13/03/2018
 *      Author: Miguel
 */

#ifndef LIB_BOOTLOADER_BOOTLOADER_H_
#define LIB_BOOTLOADER_BOOTLOADER_H_

#include <stddef.h>

#define BOOTLOADER_USER_START      0xFFFF8000UL /* FLASH_CF_BLOCK_7             */
#define BOOTLOADER_USER_BLOCKCOUNT 7            /* From 0xFFFF8000 - 0xFFFFEFFF */
#define BOOLOADER_POOL_BUFFER_SIZE 0x1000       /* Size of the transfer buffer  */

extern bool is_bootloader_busy;

#endif /* LIB_BOOTLOADER_BOOTLOADER_H_ */
