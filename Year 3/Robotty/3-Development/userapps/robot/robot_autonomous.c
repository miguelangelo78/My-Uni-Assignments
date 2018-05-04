/*
 * robot_autonomous.c
 *
 *  Created on: 16/03/2017
 *      Author: Miguel
 */
#include <globals.h>

enum ROBOT_AUTON_FSM {
	ROBOT_AUT_START,
	ROBOT_AUT_SCAN,
	ROBOT_AUT_NAVIGATE,
	ROBOT_AUT_AVOID
};

enum ROBOT_AUTON_SCAN_FSM {
	ROBOT_AUT_SCAN_LOOKFRONT = 0,
	ROBOT_AUT_SCAN_LOOKRIGHT = 7,
	ROBOT_AUT_SCAN_LOOKLEFT  = 17,
	ROBOT_AUT_SCAN__COUNT    = 3
};

enum ROBOT_AUTON_SCAN_RESULT {
	ROBOT_AUT_SCAN_RES_KEEPDRIVING,
	ROBOT_AUT_SCAN_RES_TURNRIGHT,
	ROBOT_AUT_SCAN_RES_TURNLEFT,
	ROBOT_AUT_SCAN_RES_TURNAROUND
};

enum ROBOT_AUTON_FSM         robot_aut_state       = ROBOT_AUT_START; /* This variable indicates the FSM state of the Autonomous car */
enum ROBOT_AUTON_SCAN_FSM    robot_aut_scan_state  = ROBOT_AUT_SCAN_LOOKFRONT; /* This variable indicates the FSM state of the algorithm of the environment scanner */
enum ROBOT_AUTON_SCAN_RESULT robot_aut_scan_result = ROBOT_AUT_SCAN_RES_KEEPDRIVING; /* The result of the scan algorithm. This variable will indicate what the car will do after scanning the environment */
float    robot_aut_scan_distances[ROBOT_AUT_SCAN__COUNT];
bool     robot_aut_latch_idle           = false;
bool     robot_aut_enable_usonic_bottom = true;
uint32_t robot_aut_avoid_prescaler      = 0;
uint32_t robot_aut_prescaler = (uint32_t)-1;

bool robot_auton_is_wall_detected(bool update_front) {
	float sensor_bottom   = usonic_get_avg_distance(module_usonic1);
	float sensor_top      = usonic_get_avg_distance(module_usonic2);
	bool  detected_bottom = robot_aut_enable_usonic_bottom && sensor_bottom < ROBOT_AUTON_MINIMUM1_WALL_DISTANCE_CM;
	bool  detected_top    = sensor_top < ROBOT_AUTON_MINIMUM2_WALL_DISTANCE_CM;

	if(update_front) {
		if(detected_top)
			robot_aut_scan_distances[0] = sensor_top;
		else if(detected_bottom)
			robot_aut_scan_distances[0] = sensor_bottom;
	}

	return detected_bottom || detected_top;
}

void robot_auton_start(void) {
	MTU3.TGRC = 1300; /* Quick dirt cheap fix to make the Y axis servo look down (no time left for the project to be finished). */

	/* See if there's a wall in front of the car (usonic 1) */
	if(robot_auton_is_wall_detected(false)) {
		/* If there is, enter scan mode */
		robot_aut_state = ROBOT_AUT_SCAN;
	} else {
		/* Else, enter navigate mode */
		robot_aut_state = ROBOT_AUT_NAVIGATE;
	}
}

void robot_auton_restart(void) {
	sg90_ctrl(SG90_AXIS_X, 90);
	l298n_stop();
	robot_aut_state           = ROBOT_AUT_START;
	robot_aut_scan_state      = ROBOT_AUT_SCAN_LOOKFRONT;
	robot_aut_scan_result     = ROBOT_AUT_SCAN_RES_KEEPDRIVING;
	robot_aut_latch_idle      = false;
	robot_aut_avoid_prescaler = 0;
}

void robot_auton_scan_decide(void) {
	/* Use the data collected inside the array robot_aut_scan_distances to decide
	 * whether we should go left, right or even back */
	float left  = robot_aut_scan_distances[1];
	float right = robot_aut_scan_distances[2];

	if(left > ROBOT_AUTON_MINIMUM2_WALL_DISTANCE_CM || right > ROBOT_AUTON_MINIMUM2_WALL_DISTANCE_CM) {
		/* At least one of the two sides has enough space for the car to turn */
		if(left > right)
			robot_aut_scan_result = ROBOT_AUT_SCAN_RES_TURNLEFT;  /* There is more space on the left side */
		else
			robot_aut_scan_result = ROBOT_AUT_SCAN_RES_TURNRIGHT; /* There is more space on the right side */
	} else {
		/* The car seems to be in a very tight space. It must go backward and try to make a 180º turn */
		robot_aut_scan_result = ROBOT_AUT_SCAN_RES_TURNAROUND;
	}
}

void robot_auton_scan(void) {
	switch(robot_aut_scan_state) {
	case ROBOT_AUT_SCAN_LOOKFRONT:
		sg90_ctrl(SG90_AXIS_X, 0);
		break;
	case ROBOT_AUT_SCAN_LOOKRIGHT:
		robot_aut_scan_distances[1] = usonic_get_avg_distance(module_usonic2);
		sg90_ctrl(SG90_AXIS_X, 180);
		break;
	case ROBOT_AUT_SCAN_LOOKLEFT:
		robot_aut_scan_distances[2] = usonic_get_avg_distance(module_usonic2);
		sg90_ctrl(SG90_AXIS_X, 90);
		robot_auton_scan_decide();
		robot_aut_avoid_prescaler = 0;
		robot_aut_state           = ROBOT_AUT_AVOID;
		robot_aut_scan_state      = ROBOT_AUT_SCAN_LOOKFRONT;
		return;
	default: break;
	}
	robot_aut_scan_state++;
}

void robot_auton_navigate(void) {
	l298n_stop();
	if(robot_auton_is_wall_detected(true)) {
		/* We found a wall!
		 * We must now scan the environment in order to determine the direction we will turn next. */
		robot_aut_state = ROBOT_AUT_SCAN;
		/* Set Status LED to PANIC: */
		status_led_update(module_status_led, STATUS_NOTE_PANIC, STATUS_ANIM_FASTBLINK);
		robot_aut_latch_idle = false;
	} else {
		/* Drive forward: */
		l298n_ctrl(L298N_CHANNEL_BOTH_SIDES, DIR_FORW, L298N_MOTOR_MIN_SPEED);

		/* Set Status LED to IDLE: */
		if(!robot_aut_latch_idle) {
			status_led_update(module_status_led, STATUS_NOTE_IDLE, STATUS_ANIM_SLOWFADE);
			robot_aut_latch_idle = true;
		}
	}
}

void robot_auton_avoid(void) {
	uint32_t robot_aut_avoid_prescaler_max = 7;
	if(robot_aut_scan_result == ROBOT_AUT_SCAN_RES_TURNAROUND)
		robot_aut_avoid_prescaler_max = 11;

	switch(robot_aut_scan_result) {
	case ROBOT_AUT_SCAN_RES_KEEPDRIVING:
		robot_aut_avoid_prescaler = robot_aut_avoid_prescaler_max;
		break;
	case ROBOT_AUT_SCAN_RES_TURNRIGHT:
		l298n_ctrl(L298N_CHANNEL_LEFT,  DIR_BACK, L298N_MOTOR_MIN_SPEED + 6000);
		l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_FORW, L298N_MOTOR_MIN_SPEED);
		break;
	case ROBOT_AUT_SCAN_RES_TURNLEFT:
		l298n_ctrl(L298N_CHANNEL_LEFT,  DIR_FORW, L298N_MOTOR_MIN_SPEED + 6000);
		l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_BACK, L298N_MOTOR_MIN_SPEED);
		break;
	case ROBOT_AUT_SCAN_RES_TURNAROUND:
		l298n_ctrl(L298N_CHANNEL_LEFT,  DIR_BACK, L298N_MOTOR_MIN_SPEED + 6000);
		l298n_ctrl(L298N_CHANNEL_RIGHT, DIR_FORW, L298N_MOTOR_MIN_SPEED);
		break;
	default: break;
	}

	if(++robot_aut_avoid_prescaler >= robot_aut_avoid_prescaler_max) {
		robot_aut_avoid_prescaler = 0;
		robot_aut_state           = ROBOT_AUT_NAVIGATE;
	}
}

void robot_auton_enable_bottom_usonic(void) {
	robot_aut_enable_usonic_bottom = true;
}

void robot_auton_disable_bottom_usonic(void) {
	robot_aut_enable_usonic_bottom = false;
}

void robot_auton_update(void) {
	if(robot_aut_prescaler < 40) {
		robot_aut_prescaler++;
		return;
	}
	robot_aut_prescaler = 0;

	/* Update algorithm logic: */
	switch(robot_aut_state) {
	case ROBOT_AUT_START:
		robot_auton_start();
		break;
	case ROBOT_AUT_SCAN:
		robot_auton_scan();
		break;
	case ROBOT_AUT_NAVIGATE:
		robot_auton_navigate();
		break;
	case ROBOT_AUT_AVOID:
		robot_auton_avoid();
		break;
	default: break;
	}
}
