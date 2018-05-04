/*
 * servo.c
 *
 *  Created on: 18/10/2017
 *      Author: Miguel
 */

#include <stdlib.h>
#include <utils.h>
#include <rtos_inc.h>
#include <app_config.h>
#include <pin_mapping.h>
#include "servo.h"

servo_t * servo_init(void) {
	/* Create servo handle */
	servo_t * ret   = (servo_t*)malloc(sizeof(servo_t));
	ret->angle      = 0;
	ret->angle_old  = 0;
	ret->is_locked  = true;
	ret->dev_handle = spwm_create(SERVO_FREQ, SERVO_CENTER_ANGLE_DUTY, SPWM_MODE_BOTHLVL, 0, SPWM_DEV_SERVO);

	/* Set servo PWN pin as output */
	DIR_SERVO = 1;

	return ret;
}

enum SERVO_RETCODE servo_reset(servo_t * handle) {
	if(!handle)
		return SERVO_ERR_INVAL_HANDLE;

	handle->angle     = 0;
	handle->angle_old = 0;
	handle->is_locked = true;

	return servo_ctrl(handle, 0);
}

enum SERVO_RETCODE servo_ctrl(servo_t * handle, int8_t angle) {
	if(!handle)
		return SERVO_ERR_INVAL_HANDLE;

	handle->angle_old = handle->angle; /* Store current first PWM value */
	handle->angle     = angle;         /* And update it                 */

	/* Update PWM value */
	spwm_set_duty(handle->dev_handle, mapfloat((float)angle, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, SERVO_MIN_ANGLE_DUTY, SERVO_MAX_ANGLE_DUTY));

	return SERVO_OK;
}

enum SERVO_RETCODE servo_accum_ctrl(servo_t * handle, int8_t sum_angle) {
	if(!handle)
		return SERVO_ERR_INVAL_HANDLE;

	/* Add to the current servo angle */
	return servo_ctrl(handle, sum_angle + handle->angle);
}

enum SERVO_RETCODE servo_center(servo_t * handle) {
	if(!handle)
		return SERVO_ERR_INVAL_HANDLE;

	/* Center the servo */
	return servo_ctrl(handle, 0);
}

enum SERVO_RETCODE servo_lock(servo_t * handle) {
	if(!handle)
		return SERVO_ERR_INVAL_HANDLE;

	/* Lock the servo into position */
	handle->is_locked = true;

	/* Set servo PWN pin as output */
	DIR_SERVO = 1;

	return SERVO_OK;
}

enum SERVO_RETCODE servo_unlock(servo_t * handle) {
	if(!handle)
		return SERVO_ERR_INVAL_HANDLE;

	/* Unlock the servo from its position */
	handle->is_locked = false;

	/* Set servo PWN pin as input */
	DIR_SERVO = 0;

	return SERVO_OK;
}

enum SERVO_RETCODE servo_sweep(servo_t * handle, int8_t min_angle, int8_t max_angle, uint32_t delay, int8_t increment, bool from_cur_angle) {
	if(!handle || min_angle < SERVO_MIN_ANGLE || max_angle > SERVO_MAX_ANGLE || delay == 0 || increment == 0)
		return SERVO_ERR_INVAL_ARGS;

	handle->is_sweeping = true;

	if(from_cur_angle) {
		for(int8_t angle = handle->angle; angle >= min_angle; angle -= increment) {
			servo_ctrl(handle, angle);
			rtos_delay(delay);
		}
	}

	for(int8_t angle = min_angle; angle < max_angle; angle += increment) {
		servo_ctrl(handle, angle);
		rtos_delay(delay);
	}

	servo_ctrl(handle, 0);

	handle->is_sweeping = false;

	return SERVO_OK;
}
