/*
 * pid.c
 *
 *  Created on: 08/11/2017
 *      Author: Miguel
 */

/**
 * NOTE:
 * This PID controller assumes the set point is always 0
 **/

#include "pid.h"
#include <pin_mapping.h>
#include <rtos_inc.h>

pid_t * pid_new_controller(float kp, float ki, float kd, float min_val, float max_val, uint32_t integral_windup_period) {
	pid_t * ret = (pid_t*)malloc(sizeof(pid_t));

	ret->feedback = 0.0f;

	ret->kp = kp;
	ret->ki = ki;
	ret->kd = kd;

	ret->target       = 0.0f;
	ret->proportional = 0.0f;
	ret->integral     = 0.0f;
	ret->derivative   = 0.0f;
	ret->error        = 0.0f;
	ret->last_error   = 0.0f;
	ret->output       = 0.0f;
	ret->min_val      = min_val;
	ret->max_val      = max_val;
	ret->integral_windup_period = integral_windup_period;

	ret->init = true;

	return ret;
}

float pid_control_recalculate(pid_t * pid_handle) {
	if(!pid_handle)
		return 0.0f;

	pid_handle->error        = pid_handle->target   - pid_handle->feedback;
	pid_handle->proportional = pid_handle->kp       * pid_handle->error;
	pid_handle->integral     = pid_handle->integral + pid_handle->error;
	pid_handle->derivative   = pid_handle->error    - pid_handle->last_error;

	pid_handle->output =
		pid_handle->proportional                 +
		pid_handle->ki * pid_handle->integral    +
		pid_handle->kd * pid_handle->derivative;

	/* Limit the output value */
	if(pid_handle->output < pid_handle->min_val)
		pid_handle->output = pid_handle->min_val;
	if(pid_handle->output > pid_handle->max_val)
		pid_handle->output = pid_handle->max_val;

	static int ctr = 0;
	if(ctr++ % pid_handle->integral_windup_period == 0) {
		pid_handle->integral = 0;
		ctr = 0;
	}

	pid_handle->last_error = pid_handle->error;

	return pid_handle->output;
}

bool pid_update_feedback(pid_t * pid_handle, float new_feedback, float target) {
	if(!pid_handle)
		return false;

	pid_handle->feedback = new_feedback;
	pid_handle->target   = target;

	return true;
}

bool pid_change_kp(pid_t * pid_handle, float new_kp) {
	if(!pid_handle)
		return false;
	pid_handle->kp = new_kp;
	return true;
}

bool pid_change_ki(pid_t * pid_handle, float new_ki) {
	if(!pid_handle)
		return false;
	pid_handle->ki = new_ki;
	return true;
}

bool pid_change_kd(pid_t * pid_handle, float new_kd) {
	if(!pid_handle)
		return false;
	pid_handle->kd = new_kd;
	return true;
}

bool pid_change_constants(pid_t * pid_handle, float new_kp, float new_ki, float new_kd) {
	if(!pid_handle)
		return false;

	if(!pid_change_kp(pid_handle, new_kp))
		return false;
	if(!pid_change_ki(pid_handle, new_ki))
		return false;
	if(!pid_change_kd(pid_handle, new_kd))
		return false;

	return true;
}

bool pid_reset(pid_t * pid_handle) {
	if(!pid_handle)
		return false;

	pid_handle->target       = 0.0f;
	pid_handle->proportional = 0.0f;
	pid_handle->integral     = 0.0f;
	pid_handle->derivative   = 0.0f;
	pid_handle->error        = 0.0f;
	pid_handle->last_error   = 0.0f;
	pid_handle->feedback     = 0.0f;

	return true;
}
