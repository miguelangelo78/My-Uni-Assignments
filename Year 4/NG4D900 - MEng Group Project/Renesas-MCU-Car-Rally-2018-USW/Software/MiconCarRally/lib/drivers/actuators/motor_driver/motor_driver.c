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
#include "motor_driver.h"
#include <app_config.h>

motor_t * motor_init(enum MOTOR_CHANNEL channel) {
	motor_t * ret = NULL;

	/* Check if channel is valid */
	if(!channel || channel >= MOTOR_CHANNEL__COUNT)
		return (motor_t*)MOTOR_ERR_INVAL_CHANNEL;

	/* Create channel */
	ret                  = (motor_t*)malloc(sizeof(motor_t));
	ret->side            = channel;
	ret->speed           = 0.0f;
	ret->speed_old       = 0.0f;
	ret->speed_safe_old  = 0.0f;
	ret->rpm_measured    = 0.0f;
	ret->rpm_timestamp_triggered = false;
	ret->rpm_timestamp     = 0;
	ret->rpm_timestamp_old = 0;
	ret->is_safemode       = false;

	/* Initialize channel controls */
	MSTP_MTU       = 0;    /* Disable module stop state                                           */
	MTU.TSTRA.BYTE = 0;    /* Stop MTU Counter                                                    */
	MTU3.TCR.BYTE  = 0x23; /* Select clock source (ICLK/64) and the timer that clears TCNT (TGRA) */
	MTU4.TCNT      = 0;    /* Reset timer counter                                                 */

	if(channel == MOTOR_CHANNEL_LEFT) {
		/* Initialize left channel */
		PORT7.DDR.BIT.B3 = 1; /* Set left motor's PWM pin as output                */
		PORT7.DDR.BIT.B5 = 1; /* Set left motor's rotation direction pin as output */
		PORT1.DDR.BIT.B1 = 0; /* Set left RPM Sensor as input                      */
		MTU4.TGRA        = 0;
		MTU4.TGRC        = 0;
	}

	if(channel == MOTOR_CHANNEL_RIGHT) {
		/* Initialize right channel */
		PORT7.DDR.BIT.B2 = 1; /* Set right motor's PWM pin as output                */
		PORT7.DDR.BIT.B4 = 1; /* Set right motor's rotation direction pin as output */
		PORT9.DDR.BIT.B6 = 0; /* Set right RPM Sensor as input                      */
		MTU4.TGRB        = 0;
		MTU4.TGRD        = 0;
	}

	/* Finish up initialization */

	MTU3.TGRA = MTU3.TGRC = MOTOR_MAX_PWM_SPEED;

	MTU.TOCR1A.BIT.PSYE = 1;    /* Enable synchronized PWM output mode         */
	MTU3.TMDR1.BIT.MD   = 8;    /* Set synchronized PWM output mode            */
	MTU3.TMDR1.BIT.BFA  = 1;    /* Set TGRA and TGRC to be used as buffer mode */
	MTU3.TMDR1.BIT.BFB  = 1;    /* Set TGRB and TGRD to be used as buffer mode */
	MTU4.TMDR1.BYTE     = 0;    /* Set 0 to exclude MTU3 effects               */
	MTU.TOERA.BYTE      = 0xC7; /* Enable output out of the pin                */
	MTU.TSTRA.BIT.CST3  = 1;    /* Enable timer counter                        */

	return ret;
}

motor_t * motor_init_safe(enum MOTOR_CHANNEL channel, bool enable_safemode) {
	motor_t * ret = motor_init(channel);
	if((enum MOTOR_RETCODE)ret == MOTOR_ERR_INVAL_CHANNEL)
		return ret;

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

	handle->speed             = 0.0f;
	handle->speed_old         = 0.0f;
	handle->speed_safe_old    = 0.0f;
	handle->rpm_measured      = 0.0f;
	handle->rpm_timestamp     = 0;
	handle->rpm_timestamp_old = 0;
	handle->rpm_timestamp_triggered = false;
	handle->is_braking        = false;

	if(handle->side == MOTOR_CHANNEL_LEFT) {
		MTU4.TGRA = 0;
		MTU4.TGRC = 0;
	} else {
		MTU4.TGRB = 0;
		MTU4.TGRD = 0;
	}

	return MOTOR_OK;
}

static enum MOTOR_RETCODE motor_update(motor_t * handle, bool use_differential) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	/* Poll the RPM sensor for this given motor */
	motor_rpm_sensor_poll(handle);

	if(!use_differential) {
		if(handle->side == MOTOR_CHANNEL_LEFT)
			MTU4.TGRC = MOTOR_LEFT_INVERSE  ? -handle->speed : handle->speed; /* Update left channel  */
		else
			MTU4.TGRD = MOTOR_RIGHT_INVERSE ? -handle->speed : handle->speed; /* Update right channel */

		return MOTOR_OK;
	}

	float newRPM;
	float error = handle->acceleration - handle->rpm_measured;

	/* Limit the error values */
	if(error >  MOTOR_MAX_RPM_SPEED) error =  MOTOR_MAX_RPM_SPEED;
	if(error < -MOTOR_MAX_RPM_SPEED) error = -MOTOR_MAX_RPM_SPEED;

	if(handle->is_braking) {
		/* Calculate the proportional system output for this motor channel (p only) */
		float pid_proportional = error * BRAKE_KP;
		if(pid_proportional >  MOTOR_MAX_RPM_SPEED) pid_proportional =  MOTOR_MAX_RPM_SPEED;
		if(pid_proportional < -MOTOR_MAX_RPM_SPEED) pid_proportional = -MOTOR_MAX_RPM_SPEED;

		newRPM = pid_proportional;
	} else {
		/* Calculate new RPM values */
		newRPM = handle->acceleration + error;
	}

	if(newRPM > MOTOR_MAX_RPM_SPEED)  newRPM =  MOTOR_MAX_RPM_SPEED;
	if(newRPM < -MOTOR_MAX_RPM_SPEED) newRPM = -MOTOR_MAX_RPM_SPEED;

	if(handle->side == MOTOR_CHANNEL_LEFT) {
#if MOTOR_LEFT_INVERSE == 1
		newRPM *= -1.0f;
#endif
		/* Update left channel */
		if(newRPM >= 0) {
			PORT7.DR.BYTE &= 0xEF;
			MTU4.TGRC = (long)(MOTOR_MAX_PWM_SPEED - 1) * newRPM / MOTOR_MAX_RPM_SPEED;
		} else {
			PORT7.DR.BYTE |= 0x10;
			MTU4.TGRC = (long)(MOTOR_MAX_PWM_SPEED - 1) * (-newRPM) / MOTOR_MAX_RPM_SPEED;
		}
	} else {
#if MOTOR_RIGHT_INVERSE == 1
		newRPM *= -1.0f;
#endif
		/* Update right channel */
		if(newRPM >= 0) {
			PORT7.DR.BYTE &= 0xDF;
			MTU4.TGRD = (long)(MOTOR_MAX_PWM_SPEED - 1) * newRPM / MOTOR_MAX_RPM_SPEED;
		} else {
			PORT7.DR.BYTE |= 0x20;
			MTU4.TGRD = (long)(MOTOR_MAX_PWM_SPEED - 1) * (-newRPM) / MOTOR_MAX_RPM_SPEED;
		}
	}

	return MOTOR_OK;
}

static enum MOTOR_RETCODE motor_calculate_differential(motor_t * handle, float pid_output) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	float r1 = 0, r2 = 0, r3 = 0;

	r2 = WHEEL_LENGTH / tan(RAD(abs(pid_output)));
	r1 = r2 - (WHEEL_WIDTH / 2);
	r3 = r2 + (WHEEL_WIDTH / 2);

	if(handle->side == MOTOR_CHANNEL_LEFT) {
		/* Change acceleration for this motor channel in particular */
		if(pid_output < 0)
			handle->acceleration = (r1 / r3) * handle->speed;

		/* No differential */
		if(pid_output >= 0)
			handle->acceleration = handle->speed;
	} else {
		/* No differential */
		if(pid_output <= 0)
			handle->acceleration = handle->speed;

		/* Change acceleration for this motor channel in particular */
		if(pid_output > 0)
			handle->acceleration = (r1 / r3) * handle->speed;
	}

	handle->acceleration = map(handle->acceleration, MOTOR_DEADBAND, MOTOR_MAX_PWM_SPEED, 0, MOTOR_MAX_RPM_SPEED);

	return MOTOR_OK;
}

enum MOTOR_RETCODE motor_stop(motor_t * handle) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	return motor_ctrl(handle, 0);
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

enum MOTOR_RETCODE motor_ctrl(motor_t * handle, float speed_percentage) {
	if(!handle)
		return MOTOR_ERR_INVAL_CHANNEL;

	/* Set new motor speed and update PWM value */
	if(motor_set_speed(handle, speed_percentage) == MOTOR_OK)
		return motor_update(handle, false);

	return MOTOR_ERR_SET_SPEED;
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

	/* Get the maximum allowed speed */
	int max_speed = handle->is_safemode ? (MOTOR_SAFE_MODE_LEVEL * MOTOR_MAX_PWM_SPEED) / 100 : MOTOR_MAX_PWM_SPEED;

	/* Change speed value (map from 0% -> 100% to range of MOTOR_DEADBAND -> max_speed) */
	handle->speed_old = handle->speed;
	handle->speed     = map(speed_percentage, 0, 100, MOTOR_DEADBAND, max_speed);

	return MOTOR_OK;
}

enum MOTOR_RETCODE motor_brake(motor_t * handle, bool is_breaking) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	handle->is_braking = is_breaking;

	return MOTOR_OK;
}

enum MOTOR_RETCODE motor_rpm_sensor_poll(motor_t * handle) {
	if(!handle || handle->side >= MOTOR_CHANNEL__COUNT)
		return MOTOR_ERR_INVAL_CHANNEL;

	if(handle->side == MOTOR_CHANNEL_LEFT) {
		if(rpmcounter_left_read()) {
			if(handle->rpm_timestamp_triggered == false) {
				handle->rpm_timestamp_triggered = true;

				uint32_t current_time = rtos_time();
				uint32_t elapsed_time = current_time - handle->rpm_timestamp_old;
				handle->rpm_measured = 60.0f / ((float)elapsed_time / 1000.0f);
				handle->rpm_timestamp_old = current_time;
			}
		} else {
			handle->rpm_timestamp_triggered = false;
		}
	}
	else if(rpmcounter_right_read()) {
		if(handle->rpm_timestamp_triggered == false) {
			handle->rpm_timestamp_triggered = true;

			uint32_t current_time = rtos_time();
			uint32_t elapsed_time = current_time - handle->rpm_timestamp_old;
			handle->rpm_measured = 60.0f / ((float)elapsed_time / 1000.0f);
			handle->rpm_timestamp_old = current_time;
		}
	} else {
		handle->rpm_timestamp_triggered = false;
	}

	return MOTOR_OK;
}
