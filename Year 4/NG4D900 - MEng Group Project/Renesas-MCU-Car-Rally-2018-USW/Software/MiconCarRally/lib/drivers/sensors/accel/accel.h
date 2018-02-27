/*
 * accel.h
 *
 *  Created on: 20/02/2018
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_SENSORS_ACCEL_ACCEL_H_
#define LIB_DRIVERS_SENSORS_ACCEL_ACCEL_H_

#include <stddef.h>
#include <libs/spwm/spwm.h>

typedef struct {
	spwm_t * dev_handle_x;
	spwm_t * dev_handle_y;
	float x;
	float y;
} accel_t;

accel_t * accel_init(void);

#endif /* LIB_DRIVERS_SENSORS_ACCEL_ACCEL_H_ */
