/*
 * rtos_new_lib.c
 *
 *  Created on: 06/02/2017
 *      Author: Miguel
 */

#include <stdlib.h>
#include <system/rtos_wrappers/rtos_new_lib.h>

/******************************************************************************************************************************************
 * TODO: PROPERLY ALLOCATE THE STACK WITH MALLOC! THIS SOLUTION IS UTTERLY TERRIBLE! I NEED TO FIND OUT WHY MALLOC IS BREAKING THE STACKS *
 *****************************************************************************************************************************************/

#define PROCESS_STACKSIZE       128
#define PROCESS_DEFAULTPRIORITY 5
#define PROCESS_MAXCOUNT        20

typedef struct {
	OS_TCB       tcb;
	CPU_STK_SIZE stacksize;
	CPU_STK      stack[PROCESS_STACKSIZE];
} process_t;

process_t procs[PROCESS_MAXCOUNT];
uint8_t   proc_i = 0;

void spawn_task_args_quanta(char * proc_name, OS_TASK_PTR task_ptr, void * args, uint16_t quanta) {
	OS_ERR err;

	if(proc_i >= PROCESS_MAXCOUNT) return;

	process_t * proc = &procs[proc_i++];
	proc->stacksize = PROCESS_STACKSIZE;

	OSTaskCreate((OS_TCB     *) &proc->tcb,
                 (CPU_CHAR   *) proc_name,
                 (OS_TASK_PTR ) task_ptr,
                 (void       *) args,
                 (OS_PRIO     ) PROCESS_DEFAULTPRIORITY,
                 (CPU_STK    *) &proc->stack[0],
                 (CPU_STK_SIZE) proc->stacksize / 10u,
                 (CPU_STK_SIZE) proc->stacksize,
                 (OS_MSG_QTY  ) 0u,
                 (OS_TICK     ) quanta,
                 (void       *) 0,
                 (OS_OPT      ) (OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
                 (OS_ERR     *)&err);
}

void spawn_task_args(char * proc_name, OS_TASK_PTR task_ptr, void * args) {
	spawn_task_args_quanta(proc_name, task_ptr, args, 0);
}

void spawn_task_quanta(char * proc_name, OS_TASK_PTR task_ptr, uint16_t quanta) {
	spawn_task_args_quanta(proc_name, task_ptr, 0, quanta);
}

void spawn_task(char * proc_name, OS_TASK_PTR task_ptr) {
	spawn_task_args_quanta(proc_name, task_ptr, 0, 0);
}

void rtos_preempt(void) {
	OS_ERR err;
	OSTimeDlyHMSM(0, 0, 0, 1, OS_OPT_TIME_HMSM_STRICT, &err);
}

void rtos_delay(uint32_t tick_count) {
	OS_ERR err;
	OSTimeDlyHMSM(0, 0, 0, tick_count, OS_OPT_TIME_HMSM_NON_STRICT, &err);
}
