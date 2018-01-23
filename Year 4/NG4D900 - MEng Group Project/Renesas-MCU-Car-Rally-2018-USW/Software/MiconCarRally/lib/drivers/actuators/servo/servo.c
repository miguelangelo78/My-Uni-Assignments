/*
 * servo.c
 *
 *  Created on: 18/10/2017
 *      Author: Miguel
 */

#include <stdlib.h>
#include <utils.h>
#include <rtos_inc.h>
#include "servo.h"

servo_t * servo_init(void) {
	/* Create servo handle */
	servo_t * ret  = (servo_t*)malloc(sizeof(servo_t));
	ret->angle     = 0;
	ret->angle_old = 0;
	ret->is_locked = true;

	/* Initialize servo controls */
	MSTP_MTU       = 0;    /* Disable module stop state                                           */
	MTU.TSTRA.BYTE = 0;    /* Stop MTU Counter                                                    */
	MTU3.TCR.BYTE  = 0x23; /* Select clock source (ICLK/64) and the timer that clears TCNT (TGRA) */
	MTU3.TCNT      = 0;    /* Reset timer counter                                                 */

	PORT7.DDR.BIT.B1      = 1;                         /* Set servo PWN pin as output          */
	MTU3.TGRB = MTU3.TGRD = SERVO_ANGLE_CENTER_AS_PWM; /* Make the servo align with the center */

	MTU.TOCR1A.BIT.PSYE = 1;    /* Enable synchronized PWM output mode         */
	MTU3.TMDR1.BIT.MD   = 8;    /* Set synchronized PWM output mode            */
	MTU3.TMDR1.BIT.BFA  = 1;    /* Set TGRA and TGRC to be used as buffer mode */
	MTU3.TMDR1.BIT.BFB  = 1;    /* Set TGRB and TGRD to be used as buffer mode */
	MTU4.TMDR1.BYTE     = 0;    /* Set 0 to exclude MTU3 effects               */
	MTU.TOERA.BYTE      = 0xC7; /* Enable output out of the pin                */
	MTU.TSTRA.BIT.CST3  = 1;    /* Enable timer counter                        */

	return ret;
}

enum SERVO_ERRCODE servo_reset(servo_t * handle) {
	if(!handle)
		return SERVO_ERR_INVAL_HANDLE;

	handle->angle     = 0;
	handle->angle_old = 0;
	handle->is_locked = true;

	return servo_ctrl(handle, SERVO_ANGLE_CENTER);
}

enum SERVO_ERRCODE servo_ctrl(servo_t * handle, int16_t angle) {
	if(!handle)
		return SERVO_ERR_INVAL_HANDLE;

	handle->angle_old = handle->angle; /* Store current first PWM value */
	handle->angle     = angle;         /* And update it                 */

	/* Limit the angle value */
	if(angle > SERVO_MAX_ANGLE)
		angle = SERVO_MAX_ANGLE;
	if(angle < SERVO_MIN_ANGLE)
		angle = SERVO_MIN_ANGLE;

	/* Update PWM value */
	MTU3.TGRD = SERVO_ANGLE_CENTER_AS_PWM - angle * SERVO_ANGLE_STEP;

	return SERVO_OK;
}

enum SERVO_ERRCODE servo_accum_ctrl(servo_t * handle, int16_t sum_angle) {
	if(!handle)
		return SERVO_ERR_INVAL_HANDLE;

	/* Add to the current servo angle */
	return servo_ctrl(handle, sum_angle + handle->angle);
}

enum SERVO_ERRCODE servo_center(servo_t * handle) {
	if(!handle)
		return SERVO_ERR_INVAL_HANDLE;

	/* Center the servo */
	return servo_ctrl(handle, SERVO_ANGLE_CENTER);
}

enum SERVO_ERRCODE servo_lock(servo_t * handle) {
	if(!handle)
		return SERVO_ERR_INVAL_HANDLE;

	/* Lock the servo into position */
	handle->is_locked = true;

	/* Set servo PWN pin as output */
	PORT7.DDR.BIT.B1 = 1;

	return SERVO_OK;
}

enum SERVO_ERRCODE servo_unlock(servo_t * handle) {
	if(!handle)
		return SERVO_ERR_INVAL_HANDLE;

	/* Unlock the servo from its position */
	handle->is_locked = false;

	/* Set servo PWN pin as input */
	PORT7.DDR.BIT.B1 = 1;

	return SERVO_OK;
}
