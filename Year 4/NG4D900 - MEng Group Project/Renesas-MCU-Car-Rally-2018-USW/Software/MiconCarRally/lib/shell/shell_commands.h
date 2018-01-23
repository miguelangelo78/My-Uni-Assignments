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

	for(int i = 0; i < command_count; i++)
		printf("%d - %s\n", i + 1, command_list[i].command);

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
	if(argc != 2) {
		DEBUG("ERROR: Invalid arguments provided.\nUsage: servo <lock|unlock|[0..180]>");
		return 1;
	}

	if(!strcmp(argv[1], "lock")) {
		servo_lock(module_servo);
	} else if(!strcmp(argv[1], "unlock")) {
		servo_unlock(module_servo);
	} else {
		servo_ctrl(module_servo, atoi(argv[1]));
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
