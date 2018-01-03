/*
 * sg90.h
 *
 *  Created on: 06/03/2017
 *      Author: Miguel
 */

#ifndef DRIVERS_ACTUATORS_SG90_SG90_H_
#define DRIVERS_ACTUATORS_SG90_SG90_H_

#include <stdint.h>
#include <stdbool.h>

#define SG90_MAX_X_AXIS_ANGLE 180
#define SG90_MAX_Y_AXIS_ANGLE 180

enum SG90_ERRCODE {
	SG90_OK,
	SG90_ERR_ALREADY_INIT,
	SG90_ERR_INVAL_AXIS,
	SG90_ERR_INVAL_ANGLE,
	SG90_ERR__COUNT /* How many different types of errors there are */
};

enum SG90_AXIS {
	SG90_AXIS_NULL,
	SG90_AXIS_X,
	SG90_AXIS_Y,
	SG90_BOTH_AXIS,
	SG90_AXIS__COUNT
};

enum SG90_ERRCODE sg90_init_channels(void);
enum SG90_ERRCODE sg90_ctrl(enum SG90_AXIS axis, uint8_t angle);
enum SG90_ERRCODE sg90_accum_ctrl(enum SG90_AXIS axis, int16_t sum_angle);
void              sg90_lock(enum SG90_AXIS lock_axis);
void              sg90_unlock(enum SG90_AXIS unlock_axis);

#endif /* DRIVERS_ACTUATORS_SG90_SG90_H_ */
