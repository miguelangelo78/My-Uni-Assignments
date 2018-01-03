#include "platform.h"
#include <stdio.h>
#include <rtos_inc.h>

#define KMAIN_STACKSIZE 128
static  CPU_STK mainStk[KMAIN_STACKSIZE];
static  OS_TCB  mainTCB;

extern void kmain_early(void);
extern void kmain(void * p_arg);

void main(void) {
	OS_ERR err;

	// We might want to execute something before launching the RTOS.
	// That code will be inside this function call
	kmain_early();

	// Disable interrupts
	R_BSP_InterruptsDisable();

	// Initialize RTOS
	OSInit(&err);

	// Initialize the microcontroller's components
	BSP_Init();
	CPU_Init();
	Mem_Init();

	// Create the first task, the kmain (Kernel Main)
	OSTaskCreate((OS_TCB     *)&mainTCB,
				(CPU_CHAR   *)"Main",
				(OS_TASK_PTR ) kmain,
				(void       *) 0,
				(OS_PRIO     ) 10,
				(CPU_STK    *)&mainStk[0],
				(CPU_STK_SIZE) KMAIN_STACKSIZE / 10u,
				(CPU_STK_SIZE) KMAIN_STACKSIZE,
				(OS_MSG_QTY  ) 0u,
				(OS_TICK     ) 0u,
				(void       *) 0,
				(OS_OPT      )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
				(OS_ERR     *)&err);

	// And finally, launch the RTOS!
	OSStart(&err);

	// The program should never reach this point
	while(1);
}
