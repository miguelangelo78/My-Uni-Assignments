/*
 * pid.h
 *
 *  Created on: 08/11/2017
 *      Author: Miguel
 */

#ifndef LIB_MATH_PID_PID_H_
#define LIB_MATH_PID_PID_H_

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct {
	bool     init;
	int      target;
	int      feedback;
	float    kp, ki, kd;
	float    proportional;
	float    integral;
	float    derivative;
	int      error;
	int      last_error;
	int      output;
	int      max_val;
	int      min_val;
	uint32_t integral_windup_period_default;
	uint32_t integral_windup_period;
} pid_t;

pid_t * pid_new_controller(float kp, float ki, float kd, int min_val, int max_val, uint32_t integral_windup_period);
int     pid_control_recalculate(pid_t * pid_handle);
bool    pid_update_feedback(pid_t * pid_handle, int new_feedback, int target);
bool    pid_change_kp(pid_t * pid_handle, float new_kp);
bool    pid_change_ki(pid_t * pid_handle, float new_ki);
bool    pid_change_kd(pid_t * pid_handle, float new_kd);
bool    pid_change_constants(pid_t * pid_handle, float new_kp, float new_ki, float new_kd);
bool    pid_reset(pid_t * pid_handle);

#endif /* LIB_MATH_PID_PID_H_ */
