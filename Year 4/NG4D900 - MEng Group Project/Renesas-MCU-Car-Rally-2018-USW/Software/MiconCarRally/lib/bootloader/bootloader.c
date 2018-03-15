/*
 * bootloader.c
 *
 *  Created on: 13/03/2018
 *      Author: Miguel
 */

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>
#include <rtos_inc.h>
#include <onchip/r_flash_rx/r_flash_rx_if.h>
#include "bootloader.h"

#define ENABLE_INTERRUPTS()  set_psw(get_psw() | ( 0x00010000))
#define DISABLE_INTERRUPTS() set_psw(get_psw() & (~0x00010000))

uint8_t bootloader_poolbuff[BOOLOADER_POOL_BUFFER_SIZE];
bool    is_flash_open      = false;
bool    is_bootloader_busy = false;

extern process_t rtos_procs[RTOS_PROCESS_MAXCOUNT];
extern OS_TCB    mainTCB;

extern int console_clear(int argc, char ** argv);

int bootloader_reset(int argc, char ** argv) {
	console_clear(0, NULL);
	CPU_RESET();
	return 0;
}

int bootloader_write(int argc, char ** argv) {
	if(argc != 3)
		return 1;

	flash_err_t ret;

	if(!is_flash_open) {
		if((ret = R_FLASH_Open()) != FLASH_SUCCESS)
			return (int)ret;

		is_bootloader_busy = true;

		/* First, disable RTOS preemption on the tasks that are about to be erased from ROM */
		OS_ERR rtos_kill_ret;
		DISABLE_INTERRUPTS();

		for(int i = 0; i < RTOS_PROCESS_MAXCOUNT; i++) {
			if((uint32_t)rtos_procs[i].tcb.TaskEntryAddr >= BOOTLOADER_USER_START) {
				/* Kill the process so we don't preempt to it */
				OSTaskDel(&rtos_procs[i].tcb, &rtos_kill_ret);
				if(rtos_kill_ret != OS_ERR_NONE) {
					ENABLE_INTERRUPTS();
					return (int)rtos_kill_ret;
				}
			}
		}

		if((uint32_t)mainTCB.TaskEntryAddr >= BOOTLOADER_USER_START) {
			/* Kill the main user task */
			OSTaskDel(&mainTCB, &rtos_kill_ret);
			if(rtos_kill_ret != OS_ERR_NONE) {
				ENABLE_INTERRUPTS();
				return (int)rtos_kill_ret;
			}
		}

		/* Re-enable interrupts */
		ENABLE_INTERRUPTS();

		if((ret = R_FLASH_Erase((flash_block_address_t)BOOTLOADER_USER_START, BOOTLOADER_USER_BLOCKCOUNT)) != FLASH_SUCCESS)
			return (int)ret;

		memset(bootloader_poolbuff, 0, BOOLOADER_POOL_BUFFER_SIZE);

		is_flash_open = true;

		/* We're now running only on the bootloader's RTOS tasks. We'll reset the MCU afterwards */
	}

	uint32_t block_addr = strtoul(argv[1], NULL, 0);
	uint32_t data;

	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// TODO
	data = strtoul(argv[2], NULL, 0);
	for(int i = 0; i < BOOLOADER_POOL_BUFFER_SIZE; i++)
		bootloader_poolbuff[i] = data;

	ret = R_FLASH_Write((uint32_t)bootloader_poolbuff, block_addr, BOOLOADER_POOL_BUFFER_SIZE);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////

	if(ret == 0) {
		DEBUG("AFTER");
		for(uint8_t * a = (uint8_t*)BOOTLOADER_USER_START; a < (uint8_t*)((uint32_t)BOOTLOADER_USER_START + 50); a++)
			DEBUG("> 0x%x = 0x%x", a, *a);
	}

	bootloader_reset(0, NULL);

	/* We should never reach this point */
	while(1)
		brk();

	return 0;
}
