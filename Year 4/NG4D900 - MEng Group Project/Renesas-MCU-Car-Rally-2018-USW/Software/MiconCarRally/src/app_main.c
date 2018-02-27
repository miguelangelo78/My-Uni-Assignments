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

bool log_pid        = false;
bool log_mode       = false;
bool log_speed      = false;
bool log_unrec_patt = false;

uint8_t unrec_patt          = 0xFF;
bool    unrec_patt_detected = false;

void log_unrecognized_pattern(uint8_t pattern) {
	unrec_patt = pattern;
	unrec_patt_detected = true;
}

///////////////////////////////////////////////////////////////////////////////
void status_logger_task(void * args) {

#if ENABLE_STARTSWITCH == 1 && ENABLE_MOTOR_CTRL_LEDS == 1
	bool    is_go   = false;
	uint8_t led_val = 1;
#endif

#if ENABLE_SOUND == 1
		/* Play startup tune (only once) */
		if(module_piezo && !module_piezo->is_playing) {
			while(piezo_play_song_async(module_piezo, tune_startup, arraysize(tune_startup), false) != PIEZO_SONG_DONE) {
				while(module_piezo->is_playing)
					rtos_preempt();
			}
		}
#endif

	while(1) {

		if(log_unrec_patt && unrec_patt_detected) {
			DEBUG("UNRECOGNIZED PATTERN: %d", unrec_patt);
			unrec_patt = 0xFF;
			unrec_patt_detected = false;
		}

		if(log_pid)
			DEBUG("P: %.2f I: %.2f D: %.2f    OUT: %.2f F: %.2f ERR: %.2f LERR: %.2f",
					pid_controller_current->proportional,
					pid_controller_current->integral,
					pid_controller_current->derivative,
					pid_controller_current->output,
					pid_controller_current->feedback,
					pid_controller_current->error,
					pid_controller_current->last_error
			);

		if(log_mode) {
			int turn_counter_old = 0;

			if(track.turn_counter != turn_counter_old) {
				DEBUG("MODE: %s (%d) | OLD: %s (%d) [turn # %d]", mode_string_list[track.mode], track.mode, mode_string_list[track.last_mode], track.last_mode, track.turn_counter);
				turn_counter_old = track.turn_counter;
			}
		}

		if(log_speed)
			DEBUG("Angle: %.2f  |  L: %.2f RPM (%d, %.2f duty)  |   R: %.2f RPM (%d, %.2f duty)",
				pid_controller_current->output,
				module_left_wheel->rpm_measured,
				rpmcounter_left_read(),
				module_left_wheel->speed,
				module_right_wheel->rpm_measured,
				rpmcounter_right_read(),
				module_right_wheel->speed
			);

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

	/* Update PID control system of the car */
	update_fast_control_variables();

#if ENABLE_DEBUG_LEDS == 1 && ENABLE_DEBUGGING
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
void main_app(void * args)
{
#if ENABLE_DEBUGGING == 1
	/* Enable debugging messages */
	debug_set(true);
#endif

#if ENABLE_COMMUNICATIONS == 1
	/* Initialize the communication manager */
	packetman_init();
#endif

#if ENABLE_MOTORS == 1 || ENABLE_SERVO == 1
	/* Create and initialize the DC Motors */
	module_left_wheel  = motor_init_safe(MOTOR_CHANNEL_LEFT,  ENABLE_MOTORS_SAFEMODE);
	module_right_wheel = motor_init_safe(MOTOR_CHANNEL_RIGHT, ENABLE_MOTORS_SAFEMODE);

	/* Create and initialize the line tracker sensor and servo drivers */
	module_servo = servo_init();
#endif

#if ENABLE_ACCELEROMETER == 1
	/* Create and initialize the accelerometer driver */
	module_accel = accel_init();
#endif

#if ENABLE_PID == 1
	/* Create and initialize all PID controllers */
	pid_controller_normal    = pid_new_controller(HANDLE_KP,       HANDLE_KI,       HANDLE_KD,       SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, INT_WIND_PERIOD);
	pid_controller_crankmode = pid_new_controller(CRANK_HANDLE_KP, CRANK_HANDLE_KI, CRANK_HANDLE_KD, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, INT_WIND_PERIOD);
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

#if ENABLE_STATUS_LOGGER == 1 && ENABLE_COMMUNICATIONS == 1
	/* Create Status Logger task */
	rtos_spawn_task("status_logger_task", status_logger_task);
#endif

#if ENABLE_SHELL == 1 && ENABLE_COMMUNICATIONS == 1
	/* Create Shell task */
	rtos_spawn_task("shell_task", shell_task);
#endif

#if ENABLE_DEBUG_LEDS == 1 && ENABLE_DEBUGGING == 1
	/* Initialize Debug LEDs */
	debug_leds_init();
#endif

	while(1)
	{
#if ENABLE_STARTSWITCH == 1  && ENABLE_MOTORS == 1 || ENABLE_SERVO == 1
		/* Update FSM algorithm of the car */
		car_algorithm_poll();

		/* Handle the user switch key press event */
		if(start_switch_read() && !track.race_started) {
			/* Only continue when the user releases the button */
			while(start_switch_read())
				rtos_preempt();

			/* Give the user a bit of time to release the button */
			rtos_delay(50);

			kickstart_car();
		}
#endif
		rtos_preempt();
	}
}

void main(void)
{
	/* Install fast poller for the Software UART protocol */
	OS_FastTickRegister(poller, true);

	/* Launch RTOS which starts with a main application / task */
	rtos_launch(main_app);

	while(1);
}
