/*
 * motor.h
 *
 *  Created on: 06/02/2017
 *      Author: Miguel
 */

#ifndef DRIVERS_ACTUATORS_MOTOR_H_
#define DRIVERS_ACTUATORS_MOTOR_H_

#include <stdint.h>
#include <stdbool.h>

#define L298N_MOTOR_MIN_SPEED 9500
#define L298N_MOTOR_MAX_SPEED (65536 - L298N_MOTOR_MIN_SPEED)
#define L298N_MOTOR_SAFE_MODE_LEVEL (25000)

enum L298N_ERRCODE {
	L298N_OK,
	L298N_ERR_ALREADY_INIT,
	L298N_ERR_INVAL_CHANNEL,
	L298N_ERR_INVAL_DIRECTION,
	L298N_ERR__COUNT /* How many different types of errors there are */
};

enum L298N_DIRECTION {
	DIR_NULL,
	DIR_FORW,
	DIR_BACK,
	DIR__COUNT
};

enum L298N_CHANNEL {
	L298N_CHANNEL_NULL,
	L298N_CHANNEL_LEFT,
	L298N_CHANNEL_RIGHT,
	L298N_CHANNEL_BOTH_SIDES,
	L298N_CHANNEL__COUNT
};

enum L298N_ERRCODE l298n_init(void);
void l298n_stop(void);
void l298n_stop_single_motor(enum L298N_CHANNEL side);
void l298n_resume(enum L298N_CHANNEL side);
enum L298N_ERRCODE l298n_ctrl(enum L298N_CHANNEL side, enum L298N_DIRECTION direction, uint16_t speed);
void l298n_set_safe_mode(bool enable);

#endif /* DRIVERS_ACTUATORS_MOTOR_H_ */
