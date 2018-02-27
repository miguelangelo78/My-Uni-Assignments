/*
 * rtos_new_lib.c
 *
 *  Created on: 15/10/2017
 *      Author: Miguel
 */

#include <stdlib.h>
#include <rtos_wrappers/rtos_new_lib.h>

/******************************************************************************************************************************************
 * TODO: PROPERLY ALLOCATE THE STACK WITH MALLOC! THIS SOLUTION IS UTTERLY TERRIBLE! I NEED TO FIND OUT WHY MALLOC IS BREAKING THE STACKS *
 *****************************************************************************************************************************************/

/* Main task / process properties */
#define KMAIN_STACKSIZE 128
CPU_STK mainStk[KMAIN_STACKSIZE];
OS_TCB  mainTCB;

/* Generalized task / process properties */
#define PROCESS_STACKSIZE       300
#define PROCESS_DEFAULTPRIORITY 5
#define PROCESS_MAXCOUNT        5

typedef struct {
	OS_TCB       tcb;
	CPU_STK_SIZE stacksize;
	CPU_STK      stack[PROCESS_STACKSIZE];
} process_t;

process_t procs[PROCESS_MAXCOUNT];
uint8_t   proc_i = 0;

uint32_t rtos_time_ms = 0;
uint16_t rtos_timeout_ms[RTOS_MAX_TIMEOUT_COUNTERS];

void rtos_spawn_task_args_quanta(char * proc_name, OS_TASK_PTR task_ptr, void * args, uint16_t quanta) {
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

void rtos_spawn_task_args(char * proc_name, OS_TASK_PTR task_ptr, void * args) {
	rtos_spawn_task_args_quanta(proc_name, task_ptr, args, 0);
}

void rtos_spawn_task_quanta(char * proc_name, OS_TASK_PTR task_ptr, uint16_t quanta) {
	rtos_spawn_task_args_quanta(proc_name, task_ptr, 0, quanta);
}

void rtos_spawn_task(char * proc_name, OS_TASK_PTR task_ptr) {
	rtos_spawn_task_args_quanta(proc_name, task_ptr, 0, 0);
}

void rtos_preempt(void) {
	OS_ERR err;
	OSTimeDlyHMSM(0, 0, 0, 1, OS_OPT_TIME_HMSM_STRICT, &err);
}

void rtos_delay(uint32_t tick_count) {
	OS_ERR err;
	OSTimeDlyHMSM(0, 0, 0, tick_count, OS_OPT_TIME_HMSM_NON_STRICT, &err);
}

bool rtos_get_timeout(bool update_timer, bool reset_timer, uint32_t timeout_ms, uint8_t timeout_id) {
	if(timeout_id >= RTOS_MAX_TIMEOUT_COUNTERS)
		timeout_id = RTOS_MAX_TIMEOUT_COUNTERS - 1;

	if(reset_timer) {
		for(int i = 0; i < RTOS_MAX_TIMEOUT_COUNTERS; i++)
			rtos_timeout_ms[i] = 0;
		return true;
	}

	if(timeout_ms > 0 && rtos_timeout_ms[timeout_id] == 0) {
		rtos_timeout_ms[timeout_id] = 1;
		return false;
	}

	if(update_timer) {
		for(int i = 0; i < RTOS_MAX_TIMEOUT_COUNTERS; i++)
			if(rtos_timeout_ms[i] > 0)
				rtos_timeout_ms[i]++;
		return false;
	}

	if(timeout_ms > 0 && rtos_timeout_ms[timeout_id] >= timeout_ms) {
		rtos_timeout_ms[timeout_id] = 0;
		return true;
	}

	return false;
}

uint32_t rtos_time(void) {
	return rtos_time_ms;
}

void rtos_reset_timeout_service(void) {
	rtos_get_timeout(false, true, 0, 0);
}

bool rtos_update_timeout_service(uint32_t period_update) {
	rtos_delay(1);
	rtos_time_ms++;
	return rtos_get_timeout(true, false, 0, 0);
}

void rtos_launch(OS_TASK_PTR main_task) {
	OS_ERR err;

	OSInit(&err);

	BSP_Init();
	CPU_Init();
	Mem_Init();

	OSTaskCreate((OS_TCB     *)&mainTCB,
				(CPU_CHAR   *)"Main",
				(OS_TASK_PTR ) main_task,
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

	OSStart(&err);
	while(1);
}
