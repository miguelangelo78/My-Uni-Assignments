/*
 * motor_driver.c
 *
 *  Created on: 18/10/2017
 *      Author: Miguel
 */

#include <stdlib.h>
#include <stddef.h>
#include <utils.h>
#include <rtos_inc.h>
#include <drivers/actuators/servo/servo.h>
#include <app_config.h>
#include <pin_mapping.h>
#include "motor_driver.h"

motor_t * motor_init(enum MOTOR_CHANNEL channel) {
	motor_t * ret = NULL;

	/* Check if channel is valid */
	if(!channel || channel >= MOTOR_CHANNEL__COUNT)
		return (motor_t*)MOTOR_ERR_INVAL_CHANNEL;

	/* Create channel */
	ret                    = (motor_t*)malloc(sizeof(motor_t));
	ret->dev_handle        = spwm_create(MOTOR_FREQ, 0, SPWM_MODE_BOTHLVL, 0, channel == MOTOR_CHANNEL_LEFT ? SPWM_DEV_LEFTMOTOR : SPWM_DEV_RIGHTMOTOR);
	ret->side              = channel;
	ret->speed             = 0.0f;
	ret->speed_old         = 0.0f;
	ret->speed_safe_old    = 0.0f;
	ret->max_speed         = MOTOR_MAX_PWM_SPEED;
	ret->is_safemode       = false;
	ret->rpm_measured      = 0.0f;
	ret->rpm_timestamp     = 0;
	ret->rpm_timestamp_old = 0;
	ret->rpm_timestamp_triggered = false;

	if(channel == MOTOR_CHANNEL_LEFT) {
		/* Initialize left channel */
		DIR_MOTOR_L   = 1; /* Set left motor's PWM pin as output            */
		DIR_MOTOR_LD  = 1; /* Set left motor's direction pin as output      */
		DAT_MOTOR_LD  = 0; /* Set left motor's direction pin to 0 (forward) */
		DIR_LEFT_HALL = 0; /* Set left hall effect sensor's pin as input    */
	}

	if(channel == MOTOR_CHANNEL_RIGHT) {
		/* Initialize right channel */
		DIR_MOTOR_R    = 1; /* Set right motor's PWM pin as output            */
		DIR_MOTOR_RD   = 1; /* Set right motor's direction pin as output      */
		DAT_MOTOR_RD   = 0; /* Set right motor's direction pin to 0 (forward) */
		DIR_RIGHT_HALL = 0; /* Set right hall effect sensor's pin as input    */
	}

	return ret;
}

motor_t * motor_init_safe(enum MOTOR_CHANNEL channel, bool enable_safemode) {
	motor_t * ret = motor_init(channel);
	if((enum MOTOR_RETCODE)ret == MOTOR_ERR_INVAL_CHANNEL)
		return ret;

	ret->max_speed = MOTOR_SAFE_MODE_LEVEL;

	enum MOTOR_RETCODE errcode;
	if((errcode = motor_set_safe_mode(ret, enable_safemode, false)) == MOTOR_OK) {
		return ret;
	} else {
		free(ret);
		return (motor_t *)errcode;
	}
}

enum MOTOR_RETCODE motor_reset(motor_t * handle) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	handle->is_braking        = false;
	handle->speed             = 0.0f;
	handle->speed_old         = 0.0f;
	handle->speed_safe_old    = 0.0f;
	handle->rpm_measured      = 0.0f;
	handle->rpm_timestamp     = 0;
	handle->rpm_timestamp_old = 0;
	handle->rpm_timestamp_triggered = false;

	spwm_set_duty(handle->dev_handle, 0);

	return MOTOR_OK;
}

#if ENABLE_RPM_COUNTER == 1
static enum MOTOR_RETCODE motor_rpm_sensor_poll(motor_t * handle) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	if(handle->side == MOTOR_CHANNEL_LEFT) {
		if(rpmcounter_left_read()) {
			if(handle->rpm_timestamp_triggered == false) {
				uint32_t current_time           = rtos_time();
				uint32_t elapsed_time           = current_time - handle->rpm_timestamp_old;
				handle->rpm_measured            = 60.0f / ((float)elapsed_time / 1000.0f);
				handle->rpm_timestamp_old       = current_time;
				handle->rpm_timestamp_triggered = true;

				if(handle->rpm_measured > MOTOR_MAX_RPM_SPEED) handle->rpm_measured = MOTOR_MAX_RPM_SPEED;
				if(handle->rpm_measured < 0.0f)                handle->rpm_measured = 0.0f;
			}
		} else {
			handle->rpm_timestamp_triggered = false;
		}
	}
	else {
		if(rpmcounter_right_read()) {
			if(handle->rpm_timestamp_triggered == false) {
				uint32_t current_time           = rtos_time();
				uint32_t elapsed_time           = current_time - handle->rpm_timestamp_old;
				handle->rpm_measured            = 60.0f / ((float)elapsed_time / 1000.0f);
				handle->rpm_timestamp_old       = current_time;
				handle->rpm_timestamp_triggered = true;

				if(handle->rpm_measured > MOTOR_MAX_RPM_SPEED) handle->rpm_measured = MOTOR_MAX_RPM_SPEED;
				if(handle->rpm_measured < 0.0f)                handle->rpm_measured = 0.0f;
			}
		} else {
			handle->rpm_timestamp_triggered = false;
		}
	}

	return MOTOR_OK;
}
#endif

static enum MOTOR_RETCODE motor_update(motor_t * handle, bool use_differential) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

#if ENABLE_RPM_COUNTER == 1
	/* Poll the RPM sensor for this given motor */
	motor_rpm_sensor_poll(handle);
#endif

	if(!use_differential) {
		/* Update motor WITHOUT differential */

		/* Set the direction of the rotation for this given motor first */
		if(handle->side == MOTOR_CHANNEL_LEFT) {
			/* Set direction for the left motor */
			DAT_MOTOR_LD = handle->speed >= 0.0f ? 0 : 1;
			if(MOTOR_LEFT_INVERSE)
				DAT_MOTOR_LD = !DAT_MOTOR_LD;
		} else if(handle->side == MOTOR_CHANNEL_RIGHT) {
			/* Set direction for the right motor */
			DAT_MOTOR_RD = handle->speed >= 0.0f ? 0 : 1;
			if(MOTOR_RIGHT_INVERSE)
				DAT_MOTOR_RD = !DAT_MOTOR_RD;
		}

		handle->is_stopped = handle->speed == 0.0f;

		/* And set its speed */
		if(!handle->is_stopped)
			spwm_set_duty(handle->dev_handle, fabsf(handle->speed));
		else
			spwm_set_duty(handle->dev_handle, 0);
		return MOTOR_OK;
	}

	/* Update motor WITH differential */
	float newRPM = handle->speed - handle->deceleration;

	if(newRPM >  MOTOR_MAX_PWM_SPEED) newRPM =  MOTOR_MAX_PWM_SPEED;
	if(newRPM < -MOTOR_MAX_PWM_SPEED) newRPM = -MOTOR_MAX_PWM_SPEED;

	/* Set the direction of the rotation for this given motor */
	if(handle->side == MOTOR_CHANNEL_LEFT) {
#if MOTOR_LEFT_INVERSE == 1
		newRPM *= -1.0f;
#endif
		/* Update left channel */
		if(newRPM >= 0) DAT_MOTOR_LD = 0; /* Rotate forward   */
		else            DAT_MOTOR_LD = 1; /* Rotate backwards */
	} else {
#if MOTOR_RIGHT_INVERSE == 1
		newRPM *= -1.0f;
#endif
		/* Update right channel */
		if(newRPM >= 0) DAT_MOTOR_RD = 0; /* Rotate forward   */
		else            DAT_MOTOR_RD = 1; /* Rotate backwards */
	}

	spwm_set_duty(handle->dev_handle, ((handle->is_stopped = (newRPM == 0.0f))) ? 0.0f : fabsf(newRPM));

	return MOTOR_OK;
}

static enum MOTOR_RETCODE motor_calculate_differential(motor_t * handle, float pid_output) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	if(handle->side == MOTOR_CHANNEL_LEFT) {
		if(pid_output < 0.0f) {

#if ENABLE_RPM_COUNTER == 1
			handle->deceleration = mapfloat(fabsf(pid_output), 0, SERVO_MAX_ANGLE, 0, mapfloat(handle->rpm_measured, MOTOR_DEADBAND, MOTOR_MAX_RPM_SPEED, 0, handle->speed));
#else
			handle->deceleration = mapfloat(-pid_output, 0, SERVO_MAX_ANGLE, 0, handle->speed);
#endif

		} else {
			handle->deceleration = 0.0f;
		}
	} else {

		if(pid_output > 0.0f) {
			handle->deceleration = 0.0f;

#if ENABLE_RPM_COUNTER == 1
			handle->deceleration = mapfloat(pid_output, 0, SERVO_MAX_ANGLE, 0, mapfloat(handle->rpm_measured, MOTOR_DEADBAND, MOTOR_MAX_RPM_SPEED, 0, handle->speed));
#else
			handle->deceleration = mapfloat(pid_output, 0, SERVO_MAX_ANGLE, 0, handle->speed);
#endif

		} else {
			handle->deceleration = 0.0f;
		}
	}

	return MOTOR_OK;
}

enum MOTOR_RETCODE motor_stop(motor_t * handle) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	return motor_ctrl(handle, 0);
}

enum MOTOR_RETCODE motor_stop2(motor_t * left_motor, motor_t * right_motor) {
	enum MOTOR_RETCODE ret;
	if((ret = motor_stop(left_motor)) != MOTOR_OK)
		return ret;

	return motor_stop(right_motor);
}

enum MOTOR_RETCODE motor_resume(motor_t * handle) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	return motor_ctrl(handle, handle->speed_old);
}

enum MOTOR_RETCODE motor_refresh(motor_t * handle) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	/* Just refresh the PWM value without changing the speed value
	 * (speed could've been changed somewhere else) */
	return motor_update(handle, false);
}

enum MOTOR_RETCODE motor_refresh_with_differential(motor_t * handle, float pid_output) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	/* Calculate differential and update the PWM value without changing the speed value */
	if(motor_calculate_differential(handle, pid_output) == MOTOR_OK)
		return motor_update(handle, true);

	return MOTOR_ERR_DIFFERENTIAL;
}

enum MOTOR_RETCODE motor_refresh_with_differential2(motor_t * left_motor, motor_t * right_motor, float pid_output) {
	enum MOTOR_RETCODE ret;
	if((ret = motor_refresh_with_differential(left_motor, pid_output)) != MOTOR_OK)
		return ret;

	return motor_refresh_with_differential(right_motor, pid_output);
}

enum MOTOR_RETCODE motor_ctrl(motor_t * handle, float speed_percentage) {
	if(!handle)
		return MOTOR_ERR_INVAL_CHANNEL;

	/* Set new motor speed and update PWM value */
	if(motor_set_speed(handle, speed_percentage) == MOTOR_OK)
		return motor_update(handle, false);

	return MOTOR_ERR_SET_SPEED;
}

enum MOTOR_RETCODE motor_ctrl2(motor_t * left_motor, motor_t * right_motor, float speed_percentage) {
	enum MOTOR_RETCODE ret;
	if((ret = motor_ctrl(left_motor, speed_percentage)) != MOTOR_OK)
		return ret;

	return motor_ctrl(right_motor, speed_percentage);
}

enum MOTOR_RETCODE motor_ctrl_with_differential(motor_t * handle, float speed_percentage, float pid_output) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	/* Change speed, calculate differential and update the PWM value */
	if( motor_set_speed(handle, speed_percentage)        == MOTOR_OK &&
		motor_calculate_differential(handle, pid_output) == MOTOR_OK)
	{
		return motor_update(handle, true);
	}

	return MOTOR_ERR_DIFFERENTIAL;
}

enum MOTOR_RETCODE motor_ctrl_with_differential2(motor_t * left_motor, motor_t * right_motor, float speed_percentage, float pid_output) {
	motor_ctrl_with_differential(left_motor,  speed_percentage, pid_output);
	motor_ctrl_with_differential(right_motor, speed_percentage, pid_output);
	return MOTOR_OK;
}

enum MOTOR_RETCODE motor_set_safe_mode(motor_t * handle, bool enable, bool update_speed) {
	if(!handle)
		return MOTOR_ERR_INVAL_CHANNEL;

	handle->is_safemode = enable;

	/* Update motor speed (or not) */
	if(enable) {
		handle->speed_safe_old = handle->speed;
		if(update_speed)
			motor_ctrl(handle, MOTOR_SAFE_MODE_LEVEL);
	} else {
		if(update_speed)
			motor_ctrl(handle, handle->speed_safe_old);
	}

	return MOTOR_OK;
}

enum MOTOR_RETCODE motor_set_speed(motor_t * handle, float speed_percentage) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	/* Change speed value (map from +- 0% -> +- 100% to range of +- MOTOR_DEADBAND -> +- max_speed) */
	handle->speed_old = handle->speed;

	if(speed_percentage > 0.0f)
		handle->speed = mapfloat(speed_percentage, 0, 100, MOTOR_DEADBAND, handle->max_speed);
	else if(speed_percentage < 0.0f)
		handle->speed = mapfloat(speed_percentage, -100, 0, -handle->max_speed, -MOTOR_DEADBAND);
	else
		handle->speed = 0.0f;

	return MOTOR_OK;
}

enum MOTOR_RETCODE motor_set_speed2(motor_t * left_motor, motor_t * right_motor, float speed_percentage) {
	enum MOTOR_RETCODE ret;
	if((ret = motor_set_speed(left_motor, speed_percentage)) != MOTOR_OK)
		return ret;

	return motor_set_speed(right_motor, speed_percentage);
}

enum MOTOR_RETCODE motor_set_braking(motor_t * handle, bool is_braking) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	handle->is_braking = is_braking;

	return MOTOR_OK;
}

enum MOTOR_RETCODE motor_set_braking2(motor_t * left_motor, motor_t * right_motor, bool is_braking) {
	enum MOTOR_RETCODE ret;

	if((ret = motor_set_braking(left_motor, is_braking)) != MOTOR_OK)
		return ret;

	return motor_set_braking(right_motor, is_braking);
}

enum MOTOR_RETCODE motor_set_max_speed(motor_t * handle, float new_max_speed) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	if(new_max_speed < -100.0f || new_max_speed > 100.0f)
		return MOTOR_ERR_SET_SPEED;

	handle->max_speed = new_max_speed;

	return MOTOR_OK;
}
