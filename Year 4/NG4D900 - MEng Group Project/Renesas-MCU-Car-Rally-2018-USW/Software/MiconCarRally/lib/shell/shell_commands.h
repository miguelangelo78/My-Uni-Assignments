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
#include <platform.h>
#include <rtos_inc.h>
#include <shell.h>
#include <app_config.h>
#include <app_car_control.h>

extern const cmd_t command_list[];

/*****************************************/
/* Implementation of the shell functions */
/*   (Create new shell functions here)   */
/*****************************************/

int dc_motor_control(int argc, char ** argv) {
	if(argc < 2) {
		DEBUG("Usage: motor <left|right|all> <[-100..100]>");
		return 1;
	}

	if(!strcmp(argv[1], "left")) {
		motor_ctrl(module_left_wheel, atof(argv[2]));
	} else if(!strcmp(argv[1], "right")) {
		motor_ctrl(module_right_wheel, atof(argv[2]));
	} else if(!strcmp(argv[1], "all")) {
		float speed = atof(argv[2]);
		motor_ctrl(module_left_wheel,  speed);
		motor_ctrl(module_right_wheel, speed);
	}

	return 0;
}

int servo_control(int argc, char ** argv) {
	if(argc < 2) {
		DEBUG("Usage: servo <[-90..90] <hz>>");
		return 1;
	}

	/* Disable car controls for this amount of time (ms) */
	track.timeout_disable_control = 2500;

	if(argc == 3) {
		if(!strcmp(argv[2], "hz")) spwm_set_frequency(module_servo->dev_handle, atoi(argv[1]));
		else                       servo_ctrl(module_servo, atoi(argv[1]));
	} else {
		servo_ctrl(module_servo, atoi(argv[1]));
	}

	return 0;
}

int servo_control_sweep(int argc, char ** argv) {
	if(argc == 1) {
		track.timeout_disable_control = 2500; /* Disable car controls for this amount of time (ms) */
		servo_sweep(module_servo, -90, 90, 3, 1, false);
		return 0;
	}

	if(argc != 3) {
		DEBUG("Usage: sweep <<delay> <inc>>");
		return 1;
	}

	track.timeout_disable_control = 2500; /* Disable car controls for this amount of time (ms) */
	servo_sweep(module_servo, -90, 90, (uint32_t)atoi(argv[1]), atoi(argv[2]), true);
	return 0;
}

int go(int argc, char ** argv) {
	/* Kick start the car! */
	car_kickstart();
	return 0;
}

int stop(int argc, char ** argv) {
	/* Stop the car completely */
	change_to_new_mode(MODE_WAIT_FOR_STARTSWITCH, MODE_WAIT_FOR_STARTSWITCH);
	track.race_started      = false;
	track.is_turning_lane   = false;
	track.is_turning_corner = false;
	track.next_turn         = &track.incoming_turn[0];
	track.turn_counter      = 0;

	/* Alert the user of the event */
	piezo_play(module_piezo, &note_startswitch, false);

	servo_ctrl(module_servo, 0);

	motor_stop(module_left_wheel);
	motor_stop(module_right_wheel);

	spwm_set_frequency(module_left_wheel->dev_handle,  0);
	spwm_set_frequency(module_right_wheel->dev_handle, 0);

	return 0;
}

int prop_tune(int argc, char ** argv) {
	if(argc == 2) pid_controller->kp = atof(argv[1]);
	else          return 1;
	return 0;
}

int integ_tune(int argc, char ** argv) {
	if(argc == 2) pid_controller->ki = atof(argv[1]);
	else          return 1;
	return 0;
}

int deriv_tune(int argc, char ** argv) {
	if(argc == 2) pid_controller->kd = atof(argv[1]);
	else          return 1;
	return 0;
}

int int_windup_tune(int argc, char ** argv) {
	if(argc == 2) pid_controller->integral_windup_period = atoi(argv[1]);
	else          return 1;
	return 0;
}

int play_tune(int argc, char ** argv) {
	if(argc < 3) {
		DEBUG("Usage: play <pitch> <duration>");
		return 1;
	}

	note_t note;
	note.pitch    = (float)atoi(argv[1]);
	note.duration = atoi(argv[2]);
	piezo_play(module_piezo, &note, true);

	return 0;
}

#if ENABLE_REMOTE_CONTROL_MODE == 1

int change_to_rc_mode(int argc, char ** argv) {
	// Command Format: rcmode <0|1 <0|1>>
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

/* Prototypes declared in 'src/app_main.c' */
extern bool log_pid;
extern bool log_mode;
extern bool log_speed;
extern bool log_unrec_patt;
extern bool log_momentum;

int log_set(int argc, char ** argv) {
	if(!strcmp(argv[0], "l4"))
		log_momentum = !log_momentum;
	else if(!strcmp(argv[0], "l3"))
		log_unrec_patt = !log_unrec_patt;
	else if(!strcmp(argv[0], "l2"))
		log_speed = !log_speed;
	else if(!strcmp(argv[0], "l1"))
		log_mode = !log_mode;
	else if(!strcmp(argv[0], "l"))
		log_pid = !log_pid;
	return 0;
}

int speed_set(int argc, char ** argv) {
	if(argc > 1) {
		motor_set_max_speed(module_left_wheel,  atof(argv[1]));
		motor_set_max_speed(module_right_wheel, atof(argv[1]));
		return 0;
	} else {
		return 1;
	}
}

#pragma section BTLDR
#pragma section C CBTLDR

/* Prototypes declared in 'lib/drivers/communications/protocols/packetman/packetman.c' */
extern int packetman_connect_callback(int argc, char ** argv);
extern int packetman_disconnect_callback(int argc, char ** argv);
extern int packetman_keepalive_callback(int argc, char ** argv);

/* Prototypes declared in 'lib/bootloader/bootloader.c' */
extern int bootloader_write(int argc, char ** argv);
extern int bootloader_read(int argc, char ** argv);
extern int bootloader_reset(int argc, char ** argv);

extern int command_count;

int console_clear(int argc, char ** argv) {
	putchar(27);
	printf("[2J");
	putchar(27);
	printf("[H");

	return 0;
}

int cpu_reset(int argc, char ** argv) {
	console_clear(0, NULL);
	CPU_RESET();
	return 0;
}

int help(int argc, char ** argv) {
	puts("\nHELP: supported commands");

	for(int i = 0, j = 1; i < command_count; i++)
		if((*(uint32_t*)command_list[i].command_function != 0xFFFFFFFF) && (*(uint32_t*)command_list[i].command_function != NULL) && command_list[i].packet_compatibility == PACKET_CMD)
			printf("%d - %s\n", j++, command_list[i].command);

	return 0;
}

/*****************************************/

/***************************************/
/* Add the newly created commands here */
/***************************************/
const cmd_t command_list[] = {
	{"help",  help,          PACKET_CMD},
	{"clear", console_clear, PACKET_CMD},
	{"reset", cpu_reset,     PACKET_CMD},

#if ENABLE_STARTSWITCH == 1
	{"g", go,   PACKET_CMD},
	{"s", stop, PACKET_CMD},
#endif

#if ENABLE_MOTORS == 1
	{"speed", speed_set,        PACKET_CMD},
	{"motor", dc_motor_control, PACKET_CMD},
#endif

#if ENABLE_SERVO == 1
	{"servo", servo_control,       PACKET_CMD},
	{"sweep", servo_control_sweep, PACKET_CMD},
#endif

#if ENABLE_PID == 1
	{"p",  prop_tune,        PACKET_CMD},
	{"i",  integ_tune,       PACKET_CMD},
	{"d",  deriv_tune,       PACKET_CMD},
	{"iw", int_windup_tune,  PACKET_CMD},
#endif

#if ENABLE_SOUND == 1
	{"play", play_tune, PACKET_CMD},
#endif

#if ENABLE_REMOTE_CONTROL_MODE == 1
	{"rcmode", change_to_rc_mode, PACKET_CMD},
#endif

#if ENABLE_COMMUNICATIONS == 1
	{"connect",    packetman_connect_callback,    PACKET_CONNECT   },
	{"disconnect", packetman_disconnect_callback, PACKET_DISCONNECT},
	{"keepalive",  packetman_keepalive_callback,  PACKET_KEEPALIVE },

#if ENABLE_BOOTLOADER == 1
	{"w",     bootloader_write, PACKET_CMD},
#endif
#endif

	{"l4", log_set, PACKET_CMD},
	{"l3", log_set, PACKET_CMD},
	{"l2", log_set, PACKET_CMD},
	{"l1", log_set, PACKET_CMD},
	{"l",  log_set, PACKET_CMD},

/***************************************/
	SHELL_NULL_COMMAND
};
/***************************************/

#pragma section

#endif /* LIB_SHELL_SHELL_COMMANDS_H_ */
