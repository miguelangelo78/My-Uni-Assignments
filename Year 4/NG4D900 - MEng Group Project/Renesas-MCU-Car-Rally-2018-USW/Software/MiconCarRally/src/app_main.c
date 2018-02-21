#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <rtos_inc.h>

#include <communications/protocols/suart/suart.h>
#include <communications/protocols/packetman/packetman.h>
#include <debug.h>
#include <shell.h>

#include <app_config.h>
#include <app_car_control.h>
#include <app_track_data.h>

///////////////////////////////////////////////////////////////////////////////
void master_reset(void) {
	/*****************************************/
	/** Resetting the entire system's logic **/
	/*****************************************/

	/* Reset the track logic before resetting all the controllers */
	track.mode              = MODE_NULL;
	track.last_mode         = MODE_NULL;
	track.next_mode         = MODE_NULL;
	track.is_turning_lane   = false;
	track.is_turning_corner = false;
	track.turn_counter      = 0;
	track.rcmode_persistant = 0;
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

///////////////////////////////////////////////////////////////////////////////
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
		if(ENABLE_DIPSWITCH && is_in_template_generation_mode() && template_generator_is_finished())
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

////EXECUTION FREQUENCY = ~45.18 kHz///////////////////////////////////////////
void poller() {

#if ENABLE_COMMUNICATIONS == 1
	/* Update Software UART */
	suart_poll();
#endif

#if ENABLE_DEBUG_LEDS == 1
	/* Update debug LEDs with Software PWM */
	if(!packetman_is_connected())
		debug_leds_update_pwm();
#endif

#if ENABLE_SPWM == 1
	/* Update all devices that use SPWM */
	spwm_poll_all();
#endif
}

///////////////////////////////////////////////////////////////////////////////
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

#if ENABLE_SERVO == 1
	/* Create and initialize the line tracker sensor and servo */
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

	/* Reset RTOS timeout service */
	rtos_reset_timeout_service();

#if ENABLE_SOUND == 1
	bool is_startup_tune_finished = false;
#endif

	while(1) {
		/* Update RTOS timeout service */
		rtos_update_timeout_service();

#if ENABLE_MOTORS == 1 && ENABLE_SERVO == 1
		/* Update the control algorithm of the car */
		car_control_poll();
#endif

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

				/* Alert the user of the event */
				piezo_play(module_piezo, &note_startswitch, false);

				DEBUG("\n>>>>>>>>>>>>>>\n>> !! GO !! <<\n>>>>>>>>>>>>>>");
			} else {
				/* Reset the entire program */
				master_reset();
			}
		}
#endif

#if ENABLE_SOUND == 1
		/* Play startup tune (only once) */
		if(module_piezo && !module_piezo->is_playing && !is_startup_tune_finished)
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
