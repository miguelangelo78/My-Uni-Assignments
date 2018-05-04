/*
 * robot.c
 *
 *  Created on: 25/02/2017
 *      Author: Miguel
 */
#include <globals.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "robot.h"

enum ROBOT_MODES robot_mode                = ROBOT_DEFAULT_MODE;
bool robot_mode_enable_line_follower       = false;
bool robot_mode_switch_latch               = false;
bool robot_ltracker_found_weird_line       = false;
uint32_t robot_ltracker_weird_line_timeout = 0;

/* Function Prototype: */
void robot_update_mode_logic(void);

enum BT_CODES decode_bt_cmd(uint8_t * buff, uint32_t bufflen) {
	if(bufflen) {
		if(buff[0] == '#' && buff[1] == 'b' && buff[2] == '=' && buff[4] == '#') {
			uint8_t key = buff[3];
			if((key >= '0' && key <= '9') || key == 'A')
				return (enum BT_CODES)key;
			else
				return BT_INVAL;
		} else {
			return BT_INVAL;
		}
	} else {
		return BT_INVAL;
	}
}

void robot_on_bt(uint8_t * buff, uint32_t bufflen) {

#if ROBOT_DEBUG_ENABLE == (1)
	/* Echo back the received data: */
	suart_tx_buff(module_bluetooth, buff, bufflen);
#endif

	uint8_t key = decode_bt_cmd(buff, bufflen);

	if((robot_mode == ROBOT_MODE_AUTONOMOUS || robot_mode == ROBOT_MODE_LINEFOLLOWER) && key == NONE)
		return;

	if(key != SELECT && key != START) {
		if(robot_mode != ROBOT_MODE_RCCAR) {
			robot_auton_restart();
			robot_lfollower_restart();
			status_led_update(module_status_led, STATUS_NOTE_OK, STATUS_ANIM_SLOWFADE);
			robot_mode = ROBOT_MODE_RCCAR;
		}
	} else {
		robot_mode = ROBOT_MODE_AUTONOMOUS;
		if(key == START) {
			robot_mode_enable_line_follower = true;
			robot_mode_switch_latch = false;
		} else {
			robot_mode_enable_line_follower = false;
		}
		robot_update_mode_logic();
	}

	if(robot_mode == ROBOT_MODE_RCCAR)
		robot_rcc_on_bt((enum BT_CODES)key);
}

void robot_on_ir(uint16_t code) {
	if(code != OK) {
		robot_auton_restart();
		robot_mode = ROBOT_MODE_RCCAR;
		robot_rcc_on_ir((enum IR_CODES)code);
	} else {
		robot_mode = ROBOT_MODE_AUTONOMOUS;
	}
}

bool robot_line_detected(void) {
	ltracker_line_t line = ltracker_get_line(module_ltracker);

	bool line_detected = robot_mode_enable_line_follower && !line.inval && (line.center || line.left || line.right);
	/* Special case: if the line follower algorithm is in cooldown mode, then we will return true even if the line was not found */
	if(!line_detected && robot_mode_enable_line_follower && robot_lfollower_is_line_lost_cooldown())
		return true;
	else
		return line_detected;
}

void robot_switch_mode(enum ROBOT_MODES new_mode) {
	if(new_mode == ROBOT_MODE_AUTONOMOUS) {
		if(robot_mode_switch_latch) {
			robot_mode_switch_latch = false;
			robot_lfollower_restart();
			robot_mode = ROBOT_MODE_AUTONOMOUS;
		}
	} else if(new_mode == ROBOT_MODE_LINEFOLLOWER) {
		if(!robot_mode_switch_latch) {
			robot_mode_switch_latch = true;
			robot_auton_restart();
			robot_mode = ROBOT_MODE_LINEFOLLOWER;
			status_led_update(module_status_led, STATUS_NOTE_INFO, STATUS_ANIM_FASTFADE);
		}
	}
}

void robot_update_mode_logic(void) {
	/* If the line tracker sensor is changed, we might want to switch from autonomous to Line Follower mode. */
	if(robot_mode != ROBOT_MODE_RCCAR) {
		if(robot_line_detected()) {
			/* Found a line! We must switch the Robot mode into Line Follower Mode (only if the line is valid) */
			if(robot_lfollower_check_line())
				robot_switch_mode(ROBOT_MODE_LINEFOLLOWER);
			else
				robot_switch_mode(ROBOT_MODE_AUTONOMOUS);
		} else {
			/* No line was detected. We'll switch back to Autonomous mode */
			robot_switch_mode(ROBOT_MODE_AUTONOMOUS);

			/* Deal with the logic for when the car finds a line that confuses its line following algorithm: */
			if(robot_ltracker_found_weird_line) {
				if(robot_ltracker_weird_line_timeout < ROBOT_INVALID_LINE_FOUND_TIMEOUT) {
					robot_ltracker_weird_line_timeout++;
					/* Keep driving forward and re-checking until a valid line is found */
					robot_ltracker_found_weird_line = !robot_lfollower_check_line();
					if(!robot_ltracker_found_weird_line) /* Good, we finally found a valid line */
						robot_ltracker_weird_line_timeout = 0;
				} else {
					/* Just give up on trying to look for the line */
					robot_ltracker_found_weird_line   = false;
					robot_ltracker_weird_line_timeout = 0;
				}
			}
		}
	}
	/* Else, if we're in RC Car mode, ignore Line Following and don't switch mode (until the user indicates so via Bluetooth) */
}

void robot_debug(void) {
	ltracker_line_t line  = ltracker_get_line(module_ltracker);
	accel_t       * accel = accel_get_readings();

	/* Show Line Tracker data: */
	suart_printf(module_bluetooth, "\rIR: %d%d%d ", line.left, line.center, line.right);

	if(accel) {
		/* Show Accelerometer data: */
		suart_printf(module_bluetooth, " Accel: x: %d ", accel->x);
		suart_printf(module_bluetooth, "y: %d ", accel->y);
		suart_printf(module_bluetooth, "z: %d ", accel->z);
		suart_printf(module_bluetooth, "slope: %d ", accel->slope);
	}

	/* Show Ultrasonic data: */
	suart_printf(module_bluetooth, "USonic 1: %.3f ",       usonic_get_avg_distance(module_usonic1));
	suart_printf(module_bluetooth, "USonic 2: %.3f       ", usonic_get_avg_distance(module_usonic2));
}

void start_robot(void * args) {
	/* Set initial motor speed: */
	l298n_ctrl(L298N_CHANNEL_BOTH_SIDES, DIR_FORW, L298N_MOTOR_MIN_SPEED);
	l298n_stop();

	robot_auton_disable_bottom_usonic();

	while(1) {
		/* Update the mode variable of the robot every cycle: */
		robot_update_mode_logic();

		/* Update Robot application logic: */
		switch(robot_mode) {
			case ROBOT_MODE_IDLE: /* Do nothing */ break;
			case ROBOT_MODE_AUTONOMOUS:
				robot_auton_update();
				break;
			case ROBOT_MODE_LINEFOLLOWER:
				if(!robot_lfollower_update()) {
					/* Something went wrong while updating the logic of this mode.
					 * Perhaps the car found a very weird line and didn't manage to
					 * understand its shape.
					 * In this case, we must just switch back temporarily to autonomous and only
					 * allow the car to return to line follower mode until a valid line has been found.
					 * If a valid line hasn't been found for a certain amount of time (timeout), we will
					 * just ignore that the car found a bad line, and will default into normal autonomous mode */

					robot_switch_mode(ROBOT_MODE_AUTONOMOUS);
					robot_ltracker_found_weird_line = true;
					status_led_update(module_status_led, STATUS_NOTE_WARNING, STATUS_ANIM_FASTBLINK);
				}
				break;
			case ROBOT_MODE_RCCAR: /* This mode is event driven. Nothing to update */ break;
			default: break;
		}

#if ROBOT_DEBUG_ENABLE == (1)
		robot_debug();
#endif

		rtos_delay(ROBOT_UPDATE_DELTA);
	}
}
