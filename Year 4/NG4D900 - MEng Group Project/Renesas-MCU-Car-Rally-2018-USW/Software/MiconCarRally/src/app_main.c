#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <rtos_inc.h>

#include <communications/protocols/suart/suart.h>
#include <communications/protocols/packetman/packetman.h>
#include <bootloader/bootloader.h>
#include <debug.h>
#include <shell.h>

#include <app_config.h>
#include <app_car_control.h>
#include <app_track_data.h>
#include <pin_mapping.h>

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

		if(log_pid) {
			DEBUG("P: %.2f I: %.2f D: %.2f    OUT: %d F: %d ERR: %d LERR: %d",
					pid_controller->proportional,
					pid_controller->integral,
					pid_controller->derivative,
					pid_controller->output,
					pid_controller->feedback,
					pid_controller->error,
					pid_controller->last_error
			);
		}

		if(log_mode) {
			DEBUG("MODE: %s (%d) | OLD: %s (%d) [turn # %d]", mode_string_list[track.mode], track.mode, mode_string_list[track.last_mode], track.last_mode, track.turn_counter);
		}

		if(log_speed) {
			DEBUG("Angle: %d  |  L: %.2f RPM (%d, %.2f duty)  |   R: %.2f RPM (%d, %.2f duty)",
				pid_controller->output,
				module_left_wheel->rpm_measured,
				rpmcounter_left_read(),
				module_left_wheel->dev_handle->new_duty_cycle,
				module_right_wheel->rpm_measured,
				rpmcounter_right_read(),
				module_right_wheel->dev_handle->new_duty_cycle
			);
		}

#if ENABLE_TEMPLATE_GENERATION == 1
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
void user_poller(void) {
	/* Update the controls every 2 interrupt cycles. (For CPU execution speed reasons (FPU's fault)) */
	static int control_update_delay = 0;
	if(++control_update_delay >= 2) {
		control_update_delay = 0;
		/* Update car controls and logic */
		if(module_left_wheel != NULL && module_right_wheel != NULL && module_servo != NULL) {
			/* Update PID control system of the car */
			uint8_t line_sensor = car_update_control();

			/* Update FSM algorithm of the car */
			car_algorithm_poll(line_sensor);
		}
	}

#if ENABLE_DEBUG_LEDS == 1 && ENABLE_DEBUGGING == 1
	/* Update debug LEDs with Software PWM */
	if(!packetman_is_connected())
		debug_leds_update_pwm();
#endif

#if ENABLE_SPWM == 1
	/* Update all devices that use SPWM */
	spwm_poll_all();
#endif
}

#pragma section BTLDR
////EXECUTION FREQUENCY = ~45.18 kHz///////////////////////////////////////////
void kernel_poller(void) {

#if ENABLE_COMMUNICATIONS == 1
	/* Update Software UART */
	suart_poll();

	if(is_bootloader_busy) {
		/* Brute force the motors to stop (since we don't have PWM available right now) */
		DAT_MOTOR_L = DAT_MOTOR_R = 0;
	} else {
		/* Serve the user now that we've served the kernel */
		user_poller();
	}
#endif
}
#pragma section

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
	/* Create and initialize PID controller */
	pid_controller = pid_new_controller(PID_KP, PID_KI, PID_KD, SERVO_MIN_ANGLE, SERVO_MAX_ANGLE, INT_WIND_PERIOD);
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
#if ENABLE_STARTSWITCH == 1 && (ENABLE_MOTORS == 1 || ENABLE_SERVO == 1)
		/* Handle the user switch key press event at startup */
		if(start_switch_read() && !track.race_started) {
			/* Only continue when the user releases the button */
			while(start_switch_read())
				rtos_preempt();

			/* Give the user a bit of time to release the button */
			rtos_delay(50);

			/* The race starts! */
			car_kickstart();
		}
#endif
		rtos_preempt();

#if ENABLE_PID == 1
		if(pid_controller && pid_controller->integral_windup_period++ >=  pid_controller->integral_windup_period_default) {
			pid_controller->integral_windup_period = 0;
			pid_controller->integral = 0;
		}
#endif

		if(track.timeout_disable_control > 0)
			track.timeout_disable_control--;
	}
}

void main(void)
{
	/* Install fast poller for the Software UART protocol */
	OS_FastTickRegister(kernel_poller, true);

	/* Launch RTOS which starts with a main application / task */
	rtos_launch(main_app);

	while(1);
}
