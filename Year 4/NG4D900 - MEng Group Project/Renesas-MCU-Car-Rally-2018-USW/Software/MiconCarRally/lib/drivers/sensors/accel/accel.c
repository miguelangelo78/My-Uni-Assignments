/*
 * accel.c
 *
 *  Created on: 20/02/2018
 *      Author: Miguel
 */

#include <stdlib.h>
#include <app_config.h>
#include "accel.h"

bool is_accel_init = false;

uint32_t x = 0;
uint32_t y = 0;

void xaxis_pin_callback(uint8_t pinsample) {
	if(pinsample) x++;
	else          x = 0;
}

void yaxis_pin_callback(uint8_t pinsample) {
	if(pinsample) y++;
	else          y = 0;
}

accel_t * accel_init(void) {
	if(is_accel_init)
		return NULL;

	/* Allocate and create two PWM handles for the x-y axis */
	accel_t * ret = NULL;

#if ENABLE_ACCELEROMETER == 1
	ret = (accel_t*)malloc(sizeof(accel_t));
	ret->dev_handle_x = spwm_create(200, 50, SPWM_MODE_BOTHLVL | SPWM_MODE_REPEAT | SPWM_MODE_PININPUT, xaxis_pin_callback, SPWM_DEV_ACCELX);
	ret->dev_handle_y = spwm_create(200, 50, SPWM_MODE_BOTHLVL | SPWM_MODE_REPEAT | SPWM_MODE_PININPUT, yaxis_pin_callback, SPWM_DEV_ACCELY);

	/* Set accelerometer's pins as inputs */
	DIR_ACCEL_X = DIR_ACCEL_Y = 0;

	is_accel_init = true;
#endif

	return ret;
}
