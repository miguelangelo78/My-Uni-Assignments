/*
 * robot_rc_car.c
 *
 *  Created on: 16/03/2017
 *      Author: Miguel
 */
#include <globals.h>

bool stop_toggle    = false;
bool motor_or_servo = false; /* 0: motor. 1: servo */
uint16_t base_speed = L298N_MOTOR_MIN_SPEED;

void robot_rcc_on_bt(enum BT_CODES key) {
	switch(key) {
	case NONE:
		l298n_stop();
		break;
	case UP:
		if(!motor_or_servo) {
			l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_FORW, base_speed);
			l298n_ctrl(L298N_CHANNEL_LEFT, DIR_FORW, base_speed);
		} else {
			sg90_accum_ctrl(SG90_AXIS_Y, 22);
		}
		break;
	case DOWN:
		if(!motor_or_servo) {
			l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_BACK, base_speed);
			l298n_ctrl(L298N_CHANNEL_LEFT, DIR_BACK, base_speed);
		} else {
			sg90_accum_ctrl(SG90_AXIS_Y, -22);
		}
		break;
	case LEFT:
		if(!motor_or_servo) {
			l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_FORW, base_speed);
			l298n_ctrl(L298N_CHANNEL_LEFT, DIR_BACK, base_speed);
		} else {
			sg90_accum_ctrl(SG90_AXIS_X, 5);
		}
		break;
	case RIGHT:
		if(!motor_or_servo) {
			l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_BACK, base_speed);
			l298n_ctrl(L298N_CHANNEL_LEFT, DIR_FORW, base_speed);
		} else {
			sg90_accum_ctrl(SG90_AXIS_X, -5);
		}
		break;
	case TRIANGLE:
		base_speed += 400;
		if(base_speed >= L298N_MOTOR_MAX_SPEED) base_speed = L298N_MOTOR_MAX_SPEED;
		break;
	case CIRCLE:
		motor_or_servo = false;
		break;
	case CROSS:
		if(base_speed >= 400)
			base_speed -= 400;
		break;
	case SQUARE:
		motor_or_servo = true;
		break;
	case BT_INVAL: break;
	}
}

void robot_rcc_on_ir(enum IR_CODES key) {
	switch(key) {
	case OK:
		if(!stop_toggle)
			l298n_stop();
		else
			l298n_resume(L298N_CHANNEL_BOTH_SIDES);
		stop_toggle = !stop_toggle;
		break;
	case FORW:
		stop_toggle = false;
		l298n_ctrl(L298N_CHANNEL_BOTH_SIDES, DIR_FORW, L298N_MOTOR_MIN_SPEED);
		break;
	case BACK:
		stop_toggle = false;
		l298n_ctrl(L298N_CHANNEL_BOTH_SIDES, DIR_BACK, L298N_MOTOR_MIN_SPEED);
		break;
	case IR_LEFT:
		stop_toggle = false;
		l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_FORW, L298N_MOTOR_MIN_SPEED);
		l298n_ctrl(L298N_CHANNEL_LEFT, DIR_BACK,  L298N_MOTOR_MIN_SPEED);
		break;
	case IR_RIGHT:
		stop_toggle = false;
		l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_BACK, L298N_MOTOR_MIN_SPEED);
		l298n_ctrl(L298N_CHANNEL_LEFT, DIR_FORW,  L298N_MOTOR_MIN_SPEED);
		break;
	}
}
