#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <rtos_inc.h>

#include <communications/protocols/suart/suart.h>
#include <communications/protocols/packetman/packetman.h>
#include <debug.h>
#include <shell.h>

#include <math/pid/pid.h>
#include <actuators/motor_driver/motor_driver.h>
#include <actuators/servo/servo.h>
#include <sensors/ltracker/ltracker.h>
#include <sound/piezo.h>
#include <sound/tunes.h>
#include <onchip/switch.h>
#include <onchip/led.h>

#include "app_config.h"
#include "app_track_data.h"
#include "app_template_generator.h"

motor_t * module_left_wheel  = NULL; /* Left  DC Motor module */
motor_t * module_right_wheel = NULL; /* Right DC Motor module */
servo_t * module_servo       = NULL; /* Servo module          */

pid_t * pid_controller_normal    = NULL; /* PID for controlling the servo angle and the DC motor differential */
pid_t * pid_controller_crankmode = NULL; /* PID for controlling each DC motor while in brake mode             */
pid_t * pid_controller_current   = NULL; /* What current PID controller are we using at any given time        */

piezo_t * module_piezo = NULL; /* Piezo buzzer module */

/* Read white tape on both sides of the track */
#define check_crossline(sensor_data) ((sensor_data & MASK3_3) == b11100111)

/* Read white tape only on the right side */
#define check_rightline(sensor_data) (sensor_data == b00011111 || sensor_data == b00111111 || sensor_data == b00001111)

/* Read white tape only on the left side */
#define check_leftline(sensor_data)  (sensor_data == b11111000 || sensor_data == b11111100 || sensor_data == b11111110 || sensor_data == b11110000)

/* Check if we have recovered by avoiding the left edge */
#define check_left_boundary_recovery(sensor_data) (sensor_data == b01100000 || sensor_data == b11100000)

/* Check if we have recovered by avoiding the right edge */
#define check_right_boundary_recovery(sensor_data) (sensor_data == b00000110 || sensor_data == b00000111)

#define is_in_template_generation_mode() (uint8_t)((dipswitch_read() & 0x8) >> 3)

void change_to_new_mode(enum MODE new_mode, enum MODE next_mode) {
	track.last_mode = track.mode;
	track.mode      = new_mode;
	track.next_mode = next_mode;
}

void change_to_next_mode(enum MODE next_mode) {
	track.last_mode = track.mode;
	track.mode      = track.next_mode;
	track.next_mode = next_mode;
}

bool handle_linetape_detection(uint8_t sensor_data) {
	bool ret = false;

	/* Check crossline tape which covers both lanes */
	if((ret = check_crossline(sensor_data))) {
#if ENABLE_TEMPLATE_GENERATION == 1
		template_generator_begin();
#else
		if(is_in_template_generation_mode())
			template_generator_begin();
		else
			change_to_new_mode(MODE_TURNING_CORNER, MODE_FOLLOW_NORMAL_TRACE);
#endif
	}

	/* Check white tape which covers only the right lane */
	if(!ret && (ret = check_rightline(sensor_data))) {
#if ENABLE_TEMPLATE_GENERATION == 1
		template_generator_begin();
#else
		if(is_in_template_generation_mode())
			template_generator_begin();
		else
			change_to_new_mode(MODE_FOUND_RIGHT_TAPE, MODE_NULL);
#endif
	}

	/* Check white tape which covers only the left lane */
	if(!ret && (ret = check_leftline(sensor_data))) {
#if ENABLE_TEMPLATE_GENERATION == 1
		template_generator_begin();
#else
		if(is_in_template_generation_mode())
			template_generator_begin();
		else
			change_to_new_mode(MODE_FOUND_LEFT_TAPE, MODE_NULL);
#endif
	}

	if(ret) {
		/* We found a white tape. Don't care which one for now,
		 * all we know now is that we must slow down the wheels */
		//xxx: UNCOMMENT ME LATER motor_brake(module_left_wheel,  true);
		//xxx: UNCOMMENT ME LATER motor_brake(module_right_wheel, true);
		pid_reset(pid_controller_crankmode);
	}

#if ENABLE_TEMPLATE_GENERATION == 1
	template_generator_update(sensor_data);
#else
	if(is_in_template_generation_mode())
		template_generator_update(sensor_data);
#endif

	return ret;
}

void handle_normal_drive(uint8_t sensor_data, bool update_mode, bool update_motors) {
	for(int i = 0; i < PATTERN_MAP_SIZE; i++) {
		if(sensor_data == track.pattern_map[i].pattern) {
			/* Update servo angle */
			pid_update_feedback(pid_controller_current, track.pattern_map[i].desired_angle);

			/* The mode (if necessary, depends on the map) */
			if(update_mode)
				change_to_new_mode(track.pattern_map[i].change_mode, MODE_NULL);

			/* And update the motors as well */
			if(update_motors) {
				motor_set_speed(module_left_wheel,  track.pattern_map[i].desired_motor_left);
				motor_set_speed(module_right_wheel, track.pattern_map[i].desired_motor_right);
			}

			break;
		}
	}
}

void get_next_turn(void) {
	/* Fetch the next incoming turn */
	if(track.next_turn == NULL) {
		track.next_turn = &track.incoming_turn[0];
	} else {
		if(++track.turn_counter >= LAP_MAX_TURNS) {
			track.turn_counter = 1;
			track.laps_completed++;
		}
		track.next_turn = &track.incoming_turn[track.turn_counter];
	}
}

void car_control_poll() {
	if(module_left_wheel == NULL || module_right_wheel == NULL || module_servo == NULL)
		return;

	if(rtos_time() % 50) {
		uint8_t sensor = ltracker_read(MASK4_4, NULL);

		if(sensor == b11111111 || sensor == b00000000) {
			if(++track.line_misread_danger_counter >= 5) {
				/* Completely stop the car if all of the sensor's have been on/off for this period of time */
				change_to_next_mode(MODE_WAIT_FOR_STARTSWITCH);
				motor_set_speed(module_left_wheel,  0);
				motor_set_speed(module_right_wheel, 0);

				/* Alert the user of the event */
				note_t note_alert;
				note_alert.duration = 500;
				note_alert.pitch    = A6;
				piezo_play(module_piezo, &note_alert, false);
			}
		} else {
			track.line_misread_danger_counter = 0;
		}
	}


	switch(track.mode) {
		case MODE_NULL: /* The car shouldn't react to this */ return;
		case MODE_WAIT_FOR_STARTSWITCH:
		{
			/* RTOS is going to tell us when to go
			   Meanwhile, we'll lock the servo onto the line */
			handle_normal_drive(ltracker_read(MASK4_4, NULL), false, false);
			break;
		}
		case MODE_FOLLOW_NORMAL_TRACE: ////////////////////////////////////////////////////
		{
			/* Check for a white tape on the track */
			uint8_t sensor_data = ltracker_read(MASK4_4, NULL);
			if(track.mode == MODE_RACE_COMPLETE || handle_linetape_detection(sensor_data))
				break;

			/* Drive the car normally */
			handle_normal_drive(sensor_data, true, true);
			break;
		}
		case MODE_AVOID_RIGHT_BOUNDARY: ////////////////////////////////////////////////////
		case MODE_AVOID_LEFT_BOUNDARY:
		{
			uint8_t sensor_data = ltracker_read(MASK4_4, NULL);
			if(track.mode == MODE_RACE_COMPLETE || handle_linetape_detection(sensor_data))
				break;

			if(track.mode == MODE_AVOID_RIGHT_BOUNDARY && check_right_boundary_recovery(sensor_data))
				change_to_new_mode(MODE_FOLLOW_NORMAL_TRACE, MODE_NULL);
			else if(track.mode == MODE_AVOID_LEFT_BOUNDARY && check_left_boundary_recovery(sensor_data))
				change_to_new_mode(MODE_FOLLOW_NORMAL_TRACE, MODE_NULL);
			break;
		}
		case MODE_FOUND_LEFT_TAPE: ////////////////////////////////////////////////////
		case MODE_FOUND_RIGHT_TAPE:
		{
			uint8_t sensor_data = ltracker_read(MASK3_3, NULL);
			if(track.mode == MODE_RACE_COMPLETE)
				break;

			/* Change lane to the left / right if and ONLY if we did NOT "misdetect" a crossline */
			if(sensor_data != b11100111) {
				/* Make the car follow an "imaginary" line through a lane change */
				change_to_new_mode(MODE_TURNING_CORNER, MODE_FOLLOW_NORMAL_TRACE);
			} else {
				/* Make the car follow an "imaginary" line through a 90 degree turn */
				change_to_new_mode(MODE_TURNING_CORNER, MODE_FOLLOW_NORMAL_TRACE);
			}

			/* Fetch the next incoming turn */
			get_next_turn();

			break;
		}
		case MODE_TURNING_CORNER:
		{
			/* Drive the car normally using the template sensor data */
			bool motors_updated;
			uint8_t sensor_data = ltracker_read(MASK4_4, &motors_updated);

			if(track.laps_completed >= LAP_MAX_COUNT) {
				track.mode = MODE_RACE_COMPLETE;
				motor_brake(module_left_wheel,  true);
				motor_brake(module_right_wheel, true);
				motor_set_speed(module_left_wheel,  0);
				motor_set_speed(module_right_wheel, 0);
			} else {
				handle_normal_drive(sensor_data, false, !motors_updated);
			}
			break;
		}
	}
	////////////////////////////////////////////////////

	/* Switch to the correct PID controller, which depends on whether or not we are breaking/slowing down */
	pid_controller_current = (module_left_wheel->is_braking || module_right_wheel->is_braking) ? pid_controller_crankmode : pid_controller_normal;

	/* Recalculate the PID controller values */
	float pid_output = pid_control(pid_controller_current);

	/* And update the external systems respectively */
	servo_ctrl(module_servo, (int16_t)pid_output);

	if(track.mode > MODE_WAIT_FOR_STARTSWITCH) {
		/* Only update the motors if we are NOT in null mode
		 * and  NOT waiting for the start switch  */
		motor_refresh_with_differential(module_left_wheel,  pid_output);
		motor_refresh_with_differential(module_right_wheel, pid_output);
	}
}

void master_reset(void) {
	/*****************************************/
	/** Resetting the entire system's logic **/
	/*****************************************/

	/* Reset the track logic before resetting all the controllers */
	track.mode         = MODE_NULL;
	track.last_mode    = MODE_NULL;
	track.next_mode    = MODE_NULL;
	track.turn_counter = 0;
	track.line_misread_danger_counter = 0;

	/* Reset DC motor modules */
	motor_reset(module_left_wheel);
	motor_reset(module_right_wheel);

	/* Reset servo module */
	servo_reset(module_servo);

	/* Reset all PID controllers */
	pid_reset(pid_controller_normal);
	pid_reset(pid_controller_crankmode);
	pid_controller_current = pid_controller_normal;

	/* Reset RTOS' timeout service */
	rtos_reset_timeout_service();

	/* Reset interactive shell */
	shell_reset();

	DEBUG("** ALERT: Master reset executed. Press the switch again to start. **");

	/* Reset debug LEDs */
	debug_leds_reset_all();

	/* Prevent switch debouncing */
	while(start_switch_read())
		rtos_preempt();

	/* Wait for the user to press the switch again */
	change_to_new_mode(MODE_WAIT_FOR_STARTSWITCH, MODE_FOLLOW_NORMAL_TRACE);
}

void status_logger_task(void * args) {
#if ENABLE_STARTSWITCH == 1 && ENABLE_MOTOR_CTRL_LEDS == 1
	bool    is_go   = false;
	uint8_t led_val = 1;
#endif

	while(1) {

#if ENABLE_TEMPLATE_GENERATION == 1
		if(template_generator_is_finished())
			template_generator_dump();
#else
		if(is_in_template_generation_mode() && template_generator_is_finished())
			template_generator_dump();
#endif

#if ENABLE_STARTSWITCH == 1 && ENABLE_MOTOR_CTRL_LEDS == 1
		if(start_switch_read()) {
			is_go = false;
			motor_ctrl_board_led(3);
			rtos_preempt();
			continue;
		}

		if(track.mode == MODE_WAIT_FOR_STARTSWITCH) {
			is_go   = false;
			led_val = led_val == 1 ? 2 : 1;
		} else if(!is_go && track.mode != MODE_WAIT_FOR_STARTSWITCH) {
			is_go   = true;
			led_val = 0;
		}

		if(is_go) {
			motor_ctrl_board_led(led_val);
			led_val = led_val == 0 ? 3 : 0;
			rtos_delay(50);
		} else {
			rtos_delay(500);
		}

		motor_ctrl_board_led(led_val);
#else
		rtos_preempt();
#endif
	}
}

void poller() {
	suart_poll();       /* Update Software UART                    */
	car_control_poll(); /* Update the control algorithm of the car */

#if ENABLE_DEBUG_LEDS == 1
	/* Update debug LEDs with Software PWM */
	if(!packetman_is_connected())
		debug_leds_update_pwm();
#endif

	spwm_poll();
}

void test_packet_stream(void * args) {
	while(1) {
		if(packetman_is_connected()) {
			packet_cmd_t cmd;
			cmd.getset = 3;
			cmd.dev_id = 1;
			cmd.size = 4;

			memset(cmd.operation, 0, sizeof(cmd.operation));
			sprintf(cmd.operation, "TEST");

			packetman_send_packet(&cmd, PACKET_CMD);
		}

		rtos_delay(1);
	}
}

void main_app(void * args) {

#if ENABLE_DEBUGGING == 1
	/* Enable debugging messages */
	debug_set(true);
#endif

#if ENABLE_COMMUNICATIONS == 1
	/* Initialize the communication manager */
	packetman_init();
#endif

#if ENABLE_MOTORS == 1
	/* Create and initialize the DC Motors */
	module_left_wheel  = motor_init_safe(MOTOR_CHANNEL_LEFT,  ENABLE_MOTORS_SAFEMODE);
	module_right_wheel = motor_init_safe(MOTOR_CHANNEL_RIGHT, ENABLE_MOTORS_SAFEMODE);
#endif

#if ENABLE_LTRACKER == 1
	/* Create and initialize the Line Tracker sensor */
	module_servo = servo_init();
#endif

#if ENABLE_PID == 1
	/* Create and initialize all PID controllers */
	pid_controller_normal    = pid_new_controller(HANDLE_KP,       HANDLE_KI,       HANDLE_KD);
	pid_controller_crankmode = pid_new_controller(CRANK_HANDLE_KP, CRANK_HANDLE_KI, CRANK_HANDLE_KD);
	pid_controller_current   = pid_controller_normal;
#endif

#if ENABLE_SOUND == 1
	/* Create and initialize the piezo buzzer */
	module_piezo = piezo_init();
#endif

#if ENABLE_STARTSWITCH == 1
	/* Initialize start switch GPIO pin */
	start_switch_init();
#endif

#if ENABLE_MOTOR_CTRL_LEDS == 1
	/* Initialize motor controller LEDs */
	motor_ctrl_board_led_init();
#endif

#if ENABLE_STATUS_LOGGER == 1
	/* Create Status Logger task */
	rtos_spawn_task("status_logger_task", status_logger_task);
#endif

#if ENABLE_SHELL == 1
	/* Create Shell task */
	rtos_spawn_task("shell_task", shell_task);
#endif

#if ENABLE_DEBUG_LEDS == 1
	/* Initialize Debug LEDs */
	debug_leds_init();
#endif

	rtos_spawn_task("test_packet_stream", test_packet_stream);

	/* Reset RTOS timeout service */
	rtos_reset_timeout_service();

	bool is_startup_tune_finished = false;

	while(1) {
		/* Update RTOS timeout service */
		rtos_update_timeout_service();

#if ENABLE_STARTSWITCH == 1
		/* Handle the user switch key press event */
		if(start_switch_read()) {
			/* Only continue when the user releases the button */
			while(start_switch_read()) {
				/* Keep updating the RTOS timeout service */
				rtos_update_timeout_service();
			}

			/* Give the user a bit of time to release the button */
			rtos_delay(50);

			if(track.mode == MODE_WAIT_FOR_STARTSWITCH) {
				/* Kick start the car! */
				change_to_next_mode(MODE_FOLLOW_NORMAL_TRACE);
				DEBUG("\n>>>>>>>>>>>>>>\n>> !! GO !! <<\n>>>>>>>>>>>>>>");
			} else {
				/* Reset the entire program */
				master_reset();
			}

			continue;
		}

#endif

#if ENABLE_SOUND == 1
		/* Play startup tune (only once) */
		if(!module_piezo->is_playing && !is_startup_tune_finished)
			is_startup_tune_finished = piezo_play_song_async(module_piezo, tune_startup, arraysize(tune_startup), false) == PIEZO_SONG_DONE;
#endif
	}
}

void main(void)
{
	/* Install fast poller for the Software UART protocol */
	OS_FastTickRegister(poller, 0);

	/* Launch RTOS which starts with a main application / task */
	rtos_launch(main_app);

	while(1);
}
