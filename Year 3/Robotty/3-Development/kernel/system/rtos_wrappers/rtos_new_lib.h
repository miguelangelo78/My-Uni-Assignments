/*
 * rtos_new_lib.h
 *
 *  Created on: 06/02/2017
 *      Author: Miguel
 */

#ifndef RTOS_WRAPPERS_RTOS_NEW_LIB_H_
#define RTOS_WRAPPERS_RTOS_NEW_LIB_H_

#include <rtos_inc.h>
#include <stdint.h>

void spawn_task_args_quanta(char * proc_name, OS_TASK_PTR task_ptr, void * args, uint16_t quanta);
void spawn_task_args(char * proc_name, OS_TASK_PTR task_ptr, void * args);
void spawn_task_quanta(char * proc_name, OS_TASK_PTR task_ptr, uint16_t quanta);
void spawn_task(char * proc_name, OS_TASK_PTR task_ptr);

void rtos_preempt(void);
void rtos_delay(uint32_t tick_count);

#endif /* RTOS_WRAPPERS_RTOS_NEW_LIB_H_ */
