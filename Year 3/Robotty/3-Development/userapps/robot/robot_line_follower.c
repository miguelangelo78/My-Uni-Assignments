/*
 * robot_line_follower.c
 *
 *  Created on: 16/03/2017
 *      Author: Miguel
 */

#include <globals.h>

enum COOLDOWN_SIDE {
	COOLDOWN_SIDE_NULL,
	COOLDOWN_SIDE_LEFT,
	COOLDOWN_SIDE_RIGHT
};

uint32_t robot_ltracker_lost_line_cooldown        = 0;
bool     robot_ltracker_is_cooldown               = false;
enum COOLDOWN_SIDE robot_ltracker_last_known_side = COOLDOWN_SIDE_NULL;
bool robot_ltracker_overriding_controls           = 0;

void robot_lfollower_restart(void) {
	/* Nothing to restart for now.
	 * We might want to reset some data later when we do data analysis over the
	 * sensors' data */
}

void robot_lfollower_update_last_known(enum COOLDOWN_SIDE this_side) {
	robot_ltracker_lost_line_cooldown = 0;
	robot_ltracker_is_cooldown        = true;
	robot_ltracker_last_known_side    = this_side;
}

bool robot_lfollower_update(void) {
	/* Update algorithm logic: */

	ltracker_line_t line = ltracker_get_line(module_ltracker);
	robot_ltracker_overriding_controls = false;

	/* TODO: It is a good a idea to introduce an algorithm which detects 90º (or just sharp in general) angles.
	 * This is because the only way the car could possibly make a 90º turn is to
	 * spin the two sides of the motors in opposite directions.
	 * By default, the motors are always driving forward.
	 * It is also a good idea to detect when we get off course. For this, the car must be able to know
	 * that it didn't reach the end of the track and it must correct its course by reversing the motors
	 * until the line is detected again. When the line is re-detected, we can either make a normal sharp/low angle turn,
	 * or an extreme 90º turn, all depending on what the data collected off the sensors indicates. */

	/* Two little macros to ease the reading of the next block of code: */
#define ON  true
#define OFF false

	if(line.left == OFF && line.center == OFF && line.right == OFF) {
		/* No line has been detected.
		 * If we just lost the line, we must keep driving to that last known direction, until the cooldown timer runs out */

		if(robot_ltracker_is_cooldown) {
			if(++robot_ltracker_lost_line_cooldown >= ROBOT_LOST_LINE_COOLDOWN) {
				/* Cooldown counter has timed out. We've officially lost track of the line */
				robot_ltracker_lost_line_cooldown = 0;
				robot_ltracker_is_cooldown        = false;
				robot_ltracker_last_known_side    = COOLDOWN_SIDE_NULL;
				return true;
			}

			robot_ltracker_overriding_controls = true;

			switch(robot_ltracker_last_known_side) {
			case COOLDOWN_SIDE_LEFT:
				line.left   = ON;
				line.center = OFF;
				line.right  = OFF;
				break;
			case COOLDOWN_SIDE_RIGHT:
				line.left   = OFF;
				line.center = OFF;
				line.right  = ON;
				break;
			case COOLDOWN_SIDE_NULL:
			default:
				robot_ltracker_is_cooldown        = false;
				robot_ltracker_lost_line_cooldown = 0;
				return false;
			}
		} else {
			/* Nothing to drive */
			return true;
		}
	}

	/* 7 possible combinations (excluding the one when only all LEDs are on, which is when there is no line detected): */
	if(line.left == OFF && line.center == ON && line.right == OFF) {
		/* Only the middle sensor is detecting the line.
		 * Action: Continue driving forward. */
		l298n_ctrl(L298N_CHANNEL_BOTH_SIDES, DIR_FORW, 200);

	} else if(line.left == ON && line.center == OFF && line.right == ON) {
		/* The two side sensors are detecting the line, but not the middle one?
		 * This is not acceptable.
		 * Action: The car must switch back to autonomous mode and will only
		 * return back to the line follower mode when a valid re-detection of a line occurs */
		l298n_stop();
		return false;

	} else if(line.left == ON && line.center == OFF && line.right == OFF) {
		/* Only the left sensor is detecting the line.
		 * Action: drive left at a high angle */
		if(robot_ltracker_overriding_controls)
			l298n_ctrl(L298N_CHANNEL_LEFT, DIR_BACK, 100);
		else
			l298n_stop_single_motor(L298N_CHANNEL_LEFT);
		l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_FORW, 15000);
		robot_lfollower_update_last_known(COOLDOWN_SIDE_LEFT);

	} else if(line.left == OFF && line.center == OFF && line.right == ON) {
		/* Only the right sensor is detecting the line
		 * Action: drive right at a high angle */
		l298n_ctrl(L298N_CHANNEL_LEFT,  DIR_FORW, 15000);
		if(robot_ltracker_overriding_controls)
			l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_BACK, 100);
		else
			l298n_stop_single_motor(L298N_CHANNEL_RIGHT);
		robot_lfollower_update_last_known(COOLDOWN_SIDE_RIGHT);

	} else if(line.left == OFF && line.center == ON && line.right == ON) {
		/* Both the center and right sensors are detecting a line
		 * Action: drive right at a low angle */
		l298n_ctrl(L298N_CHANNEL_LEFT,  DIR_FORW, 15000);
		if(robot_ltracker_overriding_controls)
			l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_BACK, 100);
		else
			l298n_stop_single_motor(L298N_CHANNEL_RIGHT);
		robot_lfollower_update_last_known(COOLDOWN_SIDE_RIGHT);

	} else if(line.left == ON && line.center == ON && line.right == OFF) {
		/* Both the center and left sensors are detecting a line
		 * Action: drive left at a low angle */
		if(robot_ltracker_overriding_controls)
			l298n_ctrl(L298N_CHANNEL_LEFT, DIR_BACK, 100);
		else
			l298n_stop_single_motor(L298N_CHANNEL_LEFT);
		l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_FORW, 15000);
		robot_lfollower_update_last_known(COOLDOWN_SIDE_LEFT);

	} else if(line.left == ON && line.center == ON && line.right == ON) {
		/* All sensors are detecting a line.
		 * This means the line is very wide, and we are on a valid path.
		 * Action: Continue driving forward. */
		l298n_ctrl(L298N_CHANNEL_BOTH_SIDES, DIR_FORW, 200);
	}

	return true;
}

bool robot_lfollower_check_line(void) {
	ltracker_line_t line = ltracker_get_line(module_ltracker);
	return !line.inval && !(line.left == ON && line.center == OFF && line.right == ON);
}

bool robot_lfollower_is_line_lost_cooldown(void) {
	return robot_ltracker_is_cooldown;
}
