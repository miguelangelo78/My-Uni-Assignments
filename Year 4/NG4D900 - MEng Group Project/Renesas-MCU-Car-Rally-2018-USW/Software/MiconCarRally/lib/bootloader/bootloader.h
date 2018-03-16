/*
 * bootloader.h
 *
 *  Created on: 13/03/2018
 *      Author: Miguel
 */

#ifndef LIB_BOOTLOADER_BOOTLOADER_H_
#define LIB_BOOTLOADER_BOOTLOADER_H_

#include <app_config.h>
#include <stddef.h>

#if ENABLE_BOOTLOADER == 1

#define BOOTLOADER_USER_START      0xFFFF8000UL /* FLASH_CF_BLOCK_7             */
#define BOOTLOADER_USER_BLOCKCOUNT 7            /* From 0xFFFF8000 - 0xFFFFEFFF */
#define BOOLOADER_POOL_BUFFER_SIZE 0x1000       /* Size of the transfer buffer  */

extern bool is_bootloader_busy;

#else

extern const bool is_bootloader_busy;

#endif

#endif /* LIB_BOOTLOADER_BOOTLOADER_H_ */
