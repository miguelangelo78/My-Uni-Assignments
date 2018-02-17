/*
 * shell_commands.h
 *
 *  Created on: 01/11/2017
 *      Author: Miguel
 */

#ifndef LIB_SHELL_SHELL_COMMANDS_H_
#define LIB_SHELL_SHELL_COMMANDS_H_

#include <stdbool.h>
#include <debug.h>
#include <app_config.h>
#include "shell.h"

extern cmd_t command_list[];
extern int   command_count;

/*****************************************/
/* Implementation of the shell functions */
/*   (Create new shell functions here)   */
/*****************************************/
int console_clear(int argc, char ** argv) {
	putchar(27);
	printf("[2J");
	putchar(27);
	printf("[H");

	return 0;
}

int help(int argc, char ** argv) {
	puts("\nHELP: supported commands");

	for(int i = 0; i < command_count; i++) {
		if(command_list[i].packet_compatibility == PACKET_CMD)
			printf("%d - %s\n", i + 1, command_list[i].command);
	}

	return 0;
}

#include <actuators/servo/servo.h>
#include <actuators/motor_driver/motor_driver.h>

extern motor_t * module_left_wheel;
extern motor_t * module_right_wheel;
extern servo_t * module_servo;

int dc_motor_control(int argc, char ** argv) {
	if(argc < 2) {
		DEBUG("ERROR: Invalid arguments provided.\nUsage: motor <left|right|all> <rpm|[-100..100]>");
		return 1;
	}

	if(!strcmp(argv[1], "left")) {

		if(!strcmp(argv[2], "rpm")) {
			float rpm_measured_copy = module_left_wheel->rpm_measured;
			DEBUG("Left RPM: %d", rpm_measured_copy);
		} else {
			int speed = atoi(argv[2]);
			motor_ctrl(module_left_wheel, speed);
		}
	} else if(!strcmp(argv[1], "right")) {

		if(!strcmp(argv[2], "rpm")) {
			float rpm_measured_copy = module_right_wheel->rpm_measured;
			DEBUG("Right RPM: %d", rpm_measured_copy);
		} else {
			int speed = atoi(argv[2]);
			motor_ctrl(module_right_wheel, speed);
		}
	} else if(!strcmp(argv[1], "all")) {

		if(!strcmp(argv[2], "rpm")) {
			float rpm_left_measured_copy  = module_left_wheel->rpm_measured;
			float rpm_right_measured_copy = module_right_wheel->rpm_measured;
			DEBUG("Left RPM: %d | Right RPM: %d", rpm_left_measured_copy, rpm_right_measured_copy);
		} else {
			int speed = atoi(argv[1]);
			motor_ctrl(module_left_wheel, speed);
			motor_ctrl(module_right_wheel, speed);
		}
	}

	return 0;
}

#include <platform.h>

int servo_control(int argc, char ** argv) {
	if(argc < 2) {
		DEBUG("ERROR: Invalid arguments provided.\nUsage: servo <lock|unlock|[0..180]>");
		return 1;
	}

	if(!strcmp(argv[1], "lock")) {
		servo_lock(module_servo);
	} else if(!strcmp(argv[1], "unlock")) {
		servo_unlock(module_servo);
	} else {
		if(argc == 3) {
			if(!strcmp(argv[2], "freq")) {
				DEBUG("FREQ: %d", atoi(argv[1]));
				spwm_set_frequency(module_servo->dev_handle, atoi(argv[1]));
			}
			else
				servo_ctrl(module_servo, atoi(argv[1]));
		} else {
			DEBUG("ANGLE: %d deg (%d%)", atoi(argv[1]), map(atoi(argv[1]), SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, SERVO_MIN_ANGLE_DUTY, SERVO_MAX_ANGLE_DUTY));
			servo_ctrl(module_servo, atoi(argv[1]));
		}
	}

	return 0;
}

#include <math/pid/pid.h>
extern void master_reset(void);          /* Resets the entire system's logic                                  */
extern pid_t * pid_controller_normal;    /* PID for controlling the servo angle and the DC motor differential */
extern pid_t * pid_controller_crankmode; /* PID for controlling each DC motor while in crank mode             */
extern track_t track;                    /* Current global state of the car's algorithm                       */

int go(int argc, char ** argv) {
	if(pid_reset(pid_controller_normal))
		return 1;
	if(pid_reset(pid_controller_crankmode))
		return 1;

	track.mode = MODE_FOLLOW_NORMAL_TRACE;

	return 0;
}

int reset(int argc, char ** argv) {
	master_reset();
	return 0;
}

int pid_tune(int argc, char ** argv) {
	if(!strcmp(argv[1], "p")) {
		/* pid p x */
		if(argc == 2)
			pid_controller_normal->kp = atof(argv[2]);
	} else if(!strcmp(argv[1], "i")) {
		/* pid i x */
		if(argc == 2)
			pid_controller_normal->ki = atof(argv[2]);
	} else if(!strcmp(argv[1], "d")) {
		/* pid d x */
		if(argc == 2)
			pid_controller_normal->kd = atof(argv[2]);
	} else {
		/* pid x y z */
		if(argc == 3) {
			pid_controller_normal->kp = atof(argv[1]);
			pid_controller_normal->ki = atof(argv[2]);
			pid_controller_normal->kd = atof(argv[3]);
		}
	}

	return 0;
}

int pidcrank_tune(int argc, char ** argv) {
	if(!strcmp(argv[1], "p")) {
		/* pidcrank p x */
		if(argc == 2)
			pid_controller_crankmode->kp = atof(argv[2]);
	} else if(!strcmp(argv[1], "i")) {
		/* pidcrank i x */
		if(argc == 2)
			pid_controller_crankmode->ki = atof(argv[2]);
	} else if(!strcmp(argv[1], "d")) {
		/* pidcrank d x */
		if(argc == 2)
			pid_controller_crankmode->kd = atof(argv[2]);
	} else {
		/* pidcrank x y z */
		if(argc == 3) {
			pid_controller_crankmode->kp = atof(argv[1]);
			pid_controller_crankmode->ki = atof(argv[2]);
			pid_controller_crankmode->kd = atof(argv[3]);
		}
	}

	return 0;
}

#if ENABLE_REMOTE_CONTROL_MODE == 1
extern void change_to_new_mode(enum MODE new_mode, enum MODE next_mode);
extern void change_to_next_mode(enum MODE next_mode);

int change_to_rc_mode(int argc, char ** argv) {
	// Command Format: rcmode [0|1 [0|1]?]?

	switch(argc) {
	case 1:
		/* Toggle mode from Race mode to RC mode (persistent) */
		if(track.mode != MODE_REMOTE)
			change_to_new_mode(MODE_REMOTE, track.mode);
		else
			change_to_next_mode(track.last_mode);

		track.rcmode_persistant = 0;
		break;
	case 2:
		/* Switch mode from Race mode to RC mode (arg1=0) or vice-versa (arg1=1) (persistent) */
		switch((uint8_t)argv[1]) {
		case 0: /* Switch to RC mode */
			change_to_new_mode(MODE_REMOTE, MODE_REMOTE);
			break;
		case 1: /* Switch to Race mode */
			change_to_new_mode(MODE_FOLLOW_NORMAL_TRACE, MODE_FOLLOW_NORMAL_TRACE);
			break;
		}

		track.rcmode_persistant = 0;
		break;
	case 3:
		/* Switch mode Race mode to RC mode (arg1=0) or vice-versa (arg1=1) persistant (arg2=0) or temporarily (arg2=1) */
		switch((uint8_t)argv[1]) {
		case 0: /* Switch to RC mode */
			change_to_new_mode(MODE_REMOTE, MODE_REMOTE);
			break;
		case 1: /* Switch to Race mode */
			change_to_new_mode(MODE_FOLLOW_NORMAL_TRACE, MODE_FOLLOW_NORMAL_TRACE);
			break;
		}

		track.rcmode_persistant = (bool)argv[2];
		break;
	case -1:
		/* TODO */
		break;
	}

	return 0;
}
#endif

/* Prototypes declared in 'lib/drivers/communications/protocols/packetman/packetman.c' */
extern int packetman_connect_callback(int argc, char ** argv);
extern int packetman_disconnect_callback(int argc, char ** argv);
extern int packetman_keepalive_callback(int argc, char ** argv);

/*****************************************/

/***************************************/
/* Add the newly created commands here */
/***************************************/
cmd_t command_list[] = {
	{"help",     help,          PACKET_CMD},
	{"clear",    console_clear, PACKET_CMD},

#if ENABLE_MOTORS == 1
	{"motor", dc_motor_control, PACKET_CMD},
#endif

#if ENABLE_SERVO == 1
	{"servo", servo_control, PACKET_CMD},
#endif

#if ENABLE_STARTSWITCH == 1
	{"go", go, PACKET_CMD},
#endif

	{"reset", reset, PACKET_CMD},

#if ENABLE_PID == 1
	{"pid",      pid_tune,      PACKET_CMD},
	{"pidcrank", pidcrank_tune, PACKET_CMD},
#endif

#if ENABLE_REMOTE_CONTROL_MODE == 1
	{"rcmode", change_to_rc_mode, PACKET_CMD},
#endif

#if ENABLE_COMMUNICATIONS == 1
	{"connect",    packetman_connect_callback,    PACKET_CONNECT   },
	{"disconnect", packetman_disconnect_callback, PACKET_DISCONNECT},
	{"keepalive",  packetman_keepalive_callback,  PACKET_KEEPALIVE },
#endif

/***************************************/
	SHELL_NULL_COMMAND
};
/***************************************/

#endif /* LIB_SHELL_SHELL_COMMANDS_H_ */
