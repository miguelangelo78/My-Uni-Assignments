/*
 * rtos_new_lib.h
 *
 *  Created on: 15/10/2017
 *      Author: Miguel
 */

#ifndef LIB_RTOS_RTOS_WRAPPERS_RTOS_NEW_LIB_H_
#define LIB_RTOS_RTOS_WRAPPERS_RTOS_NEW_LIB_H_

#include <stdint.h>
#include <stdbool.h>
#include <rtos_inc.h>

#define RTOS_MAX_TIMEOUT_COUNTERS 10

void rtos_spawn_task_args_quanta(char * proc_name, OS_TASK_PTR task_ptr, void * args, uint16_t quanta);
void rtos_spawn_task_args(char * proc_name, OS_TASK_PTR task_ptr, void * args);
void rtos_spawn_task_quanta(char * proc_name, OS_TASK_PTR task_ptr, uint16_t quanta);
void rtos_spawn_task(char * proc_name, OS_TASK_PTR task_ptr);

void rtos_preempt(void);
void rtos_delay(uint32_t tick_count);

uint32_t rtos_time(void);

bool rtos_get_timeout(bool update_timer, bool reset_timer, uint32_t timeout_ms, uint8_t timeout_id);
void rtos_reset_timeout_service(void);
bool rtos_update_timeout_service(uint32_t period_update);

void rtos_launch(OS_TASK_PTR main_task);

#endif /* LIB_RTOS_RTOS_WRAPPERS_RTOS_NEW_LIB_H_ */
