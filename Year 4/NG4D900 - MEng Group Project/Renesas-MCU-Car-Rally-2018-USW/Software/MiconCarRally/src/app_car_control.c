/*
 * app_car_control.c
 *
 *  Created on: 20/02/2018
 *      Author: Miguel
 */

#include <debug.h>
#include <app_car_control.h>

motor_t * module_left_wheel  = NULL; /* Left  DC Motor module */
motor_t * module_right_wheel = NULL; /* Right DC Motor module */
servo_t * module_servo       = NULL; /* Servo module          */
accel_t * module_accel       = NULL; /* Accelerometer module  */

pid_t * pid_controller_normal    = NULL; /* PID for controlling the servo angle and the DC motor differential */
pid_t * pid_controller_crankmode = NULL; /* PID for controlling each DC motor while in brake mode             */
pid_t * pid_controller_current   = NULL; /* What current PID controller are we using at any given time        */

piezo_t * module_piezo = NULL; /* Piezo buzzer module */

#define READ_LINE(sensor_var) ((sensor_var = ltracker_read(MASK4_4, NULL)) || 1)

///////////////////////////////////////////////////////////////////////////////
void update_fast_control_variables(void)
{
	float pid_output = 0.0f;

#if ENABLE_PID == 1
	/* Switch to the correct PID controller, which depends on whether or not we are breaking/slowing down */
	pid_controller_current = (module_left_wheel->is_braking || module_right_wheel->is_braking) ? pid_controller_crankmode : pid_controller_normal;

	/* Recalculate PID with values that are being set by the RTOS */
	if(!module_left_wheel->is_braking && !module_right_wheel->is_braking)
		pid_update_feedback(pid_controller_current, map_sensor_to_angle(ltracker_read(MASK4_4, NULL)), 0);

	pid_output = pid_control_recalculate(pid_controller_current);
#endif

#if ENABLE_SERVO == 1 && ENABLE_MOTORS == 1
	/* Update the external systems respectively */
	if(!module_servo->is_sweeping) {
		static int servo_update_ctr = 0;

		if(servo_update_ctr++ > 905) {
			servo_update_ctr = 0;

			/* Update the servo's duty cycle */
			servo_ctrl(module_servo, pid_output);
		}
	} else {
		/* Stop the car while the servo is sweeping */
		motor_set_speed2(module_left_wheel, module_right_wheel, 0);
	}

	/* Also update the motors' differential */
	if(track.mode > MODE_WAIT_FOR_STARTSWITCH && !module_left_wheel->is_braking && !module_right_wheel->is_braking) {
		static bool update_right_motor = false;

		if(update_right_motor)
			motor_refresh_with_differential(module_left_wheel, pid_output);
		else
			motor_refresh_with_differential(module_right_wheel, pid_output);

		update_right_motor = !update_right_motor;
	}
#endif
}

///////////////////////////////////////////////////////////////////////////////
void update_track_status(bool is_next_turn_a_lane_change)
{
	track.is_turning_lane   = is_next_turn_a_lane_change;
	track.is_turning_corner = !is_next_turn_a_lane_change;

	/* Fetch the next incoming turn */
	if(track.next_turn == NULL) {
		track.next_turn = &track.incoming_turn[0];
	} else {
		if(++track.turn_counter >= LAP_MAX_TURNS) {
			/* We have completed a lap */
			track.turn_counter = 0;

			/* Check if we have completed all laps */
			if(++track.laps_completed >= LAP_MAX_COUNT) {
				track.mode = MODE_RACE_COMPLETE;
				motor_set_braking2(module_right_wheel, module_right_wheel, true);
				motor_set_speed2(module_left_wheel, module_right_wheel, 0);

				/* Play song signaling the end of the race */
				while(piezo_play_song_async_backwards(module_piezo, tune_startup, arraysize(tune_startup), true) != PIEZO_SONG_DONE)
					rtos_preempt();
				while(piezo_play_song_async(module_piezo, tune_startup, arraysize(tune_startup), true) != PIEZO_SONG_DONE)
					rtos_preempt();
			}
		}
		track.next_turn = &track.incoming_turn[track.turn_counter];
	}
}

///////////////////////////////////////////////////////////////////////////////
enum MODE get_next_turn_mode_from_intel(bool is_next_turn_a_lane_change)
{
	if(is_next_turn_a_lane_change) {
		switch(track.intelligence_level) {
		case INTEL_BASIC:    return MODE_TURNING_LANE;
		case INTEL_ADVANCED: return MODE_ALIGN_BOUNDARY;
		case INTEL_SMART:    return MODE_TURNING_CORNER_BLIND;
		default:             return MODE_NULL;
		}
	} else {
		switch(track.intelligence_level) {
		case INTEL_BASIC:    return MODE_TURNING_CORNER;
		case INTEL_ADVANCED: return MODE_ALIGN_BOUNDARY;
		case INTEL_SMART:    return MODE_TURNING_CORNER_BLIND;
		default:             return MODE_NULL;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
bool change_mode_on_line_detection(uint8_t sensor_data)
{
	bool ret = false;

	/* Check crossline tape which covers both lanes */
	if((ret = check_crossline(sensor_data))) {
#if ENABLE_TEMPLATE_GENERATION == 1
		template_generator_begin();
#else
		if(ENABLE_DIPSWITCH && is_in_template_generation_mode())
			template_generator_begin();
		else
			change_to_new_mode(get_next_turn_mode_from_intel(false), MODE_FOLLOW_NORMAL_TRACE);
#endif
	}

	/* Check white tape which covers only the right lane */
	if(!ret && (ret = check_rightline(sensor_data))) {
#if ENABLE_TEMPLATE_GENERATION == 1
		template_generator_begin();
#else
		if(ENABLE_DIPSWITCH && is_in_template_generation_mode())
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
		if(ENABLE_DIPSWITCH && is_in_template_generation_mode())
			template_generator_begin();
		else
			change_to_new_mode(MODE_FOUND_LEFT_TAPE, MODE_NULL);
#endif
	}

	if(ret) {
		/* We found a white tape. Don't care which one for now,
		 * all we know now is that we must slow down the wheels */
		piezo_play(module_piezo, &note_turn_found, false);
		update_track_status(track.next_turn->is_lane_change);
		rtos_delay(50);

		if(track.intelligence_level != INTEL_SMART)
			motor_set_braking2(module_left_wheel, module_right_wheel, true);

		pid_reset(pid_controller_crankmode);
	}

#if ENABLE_TEMPLATE_GENERATION == 1
	template_generator_update(sensor_data);
#else
	if(ENABLE_DIPSWITCH && is_in_template_generation_mode())
		template_generator_update(sensor_data);
#endif

	return ret;
}

///////////////////////////////////////////////////////////////////////////////
float map_sensor_to_angle(uint8_t sensor_data)
{
	for(int i = 0; i < PATTERN_MAP_SIZE; i++)
		if(sensor_data == track.pattern_map[i].pattern) {
			if(track.mode != MODE_ACCIDENT) {
				motor_set_speed(module_left_wheel,  track.pattern_map[i].desired_motor_left);
				motor_set_speed(module_right_wheel, track.pattern_map[i].desired_motor_right);
			}
			return track.pattern_map[i].mapped_angle;
		}
	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void handle_normal_drive(uint8_t sensor_data, bool update_mode, bool update_motors)
{
	for(int i = 0; i < PATTERN_MAP_SIZE; i++) {
		if(sensor_data == track.pattern_map[i].pattern) {
#if ENABLE_DYNAMIC_PID == 1
			/* Update the PID coefficients for every different pattern */
			pid_change_constants(pid_controller_current, track.pattern_map[i].p, track.pattern_map[i].i, track.pattern_map[i].d);
#endif

			/* Update the mode (if necessary, depends on the map) */
			if(update_mode)
				change_to_new_mode(track.pattern_map[i].change_mode, MODE_NULL);

			return;
		}
	}

	log_unrecognized_pattern(sensor_data);
}

///////////////////////////////////////////////////////////////////////////////
void kickstart_car(void) {
	if(track.mode == MODE_WAIT_FOR_STARTSWITCH) {
		/* Kick start the car! */
		spwm_set_frequency(module_left_wheel->dev_handle,  MOTOR_FREQ);
		spwm_set_frequency(module_right_wheel->dev_handle, MOTOR_FREQ);

		change_to_new_mode(MODE_FOLLOW_NORMAL_TRACE, MODE_FOLLOW_NORMAL_TRACE);
		motor_set_braking2(module_left_wheel, module_right_wheel, false);

		track.race_started = true;

		/* Alert the user of the event */
		piezo_play(module_piezo, &note_startswitch, false);

		DEBUG("\n>>>>>>>>>>>>>>\n>> !! GO !! <<\n>>>>>>>>>>>>>>");
	}
}

///////////////////////////////////////////////////////////////////////////////
void car_algorithm_poll(void)
{
	if(module_left_wheel == NULL || module_right_wheel == NULL || module_servo == NULL)
		return;

	/* We have finished the race. Do not update the controls anymore. */
	if(track.mode == MODE_RACE_COMPLETE)
		return;

	/* Read the infrared sensor with no mask */
	uint8_t sensor_unmsk = ltracker_read(MASK4_4, NULL);

	/* Check for off-track accidents */
	if(rtos_time() % 50 == 0) {
		if(!track.is_turning_lane && track.mode > MODE_WAIT_FOR_STARTSWITCH && (sensor_unmsk == b11111111 || sensor_unmsk == b00000000)) {
			if(++track.line_misread_danger_counter >= 5)
				change_to_new_mode(MODE_ACCIDENT, MODE_ACCIDENT);
		} else {
			track.line_misread_danger_counter = 0;
		}
	}

	///// MAIN CONTROL FINITE STATE MACHINE ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	switch(track.mode) {
		case MODE_NULL: /* The car shouldn't react to this */ break;
		case MODE_WAIT_FOR_STARTSWITCH: ////APPLIES TO: BASIC|ADVANCED|SMART////////////////////////////////////////////////
		{
			/* Wait until the user presses the start button.
			   Meanwhile, we'll lock the servo onto the line */
			handle_normal_drive(sensor_unmsk, false, false);
			break;
		}
		case MODE_ACCIDENT:
		{
			/* Completely stop the car and center the servo if all of the
			 * sensors have been on/off for a certain amount of time */
			motor_stop(module_left_wheel);
			motor_stop(module_right_wheel);
			motor_set_braking2(module_left_wheel, module_left_wheel, false);
			servo_ctrl(module_servo, 0);

			/* Block the control system from updating its variables and alert the user of the event */
			piezo_stop(module_piezo, true);

			uint8_t start_switch = 0;

			while(track.mode == MODE_ACCIDENT && !start_switch) {
				piezo_play(module_piezo, &note_alert, false);
				while(module_piezo->is_playing && track.mode != MODE_ACCIDENT && !(start_switch = start_switch_read()))
					rtos_preempt();

				for(int i = 0; i < 150 && !(start_switch = start_switch_read()); i++)
					rtos_delay(1);
			}

			/* (The car better be back on the track when we reach this point...) */
			change_to_next_mode(MODE_FOLLOW_NORMAL_TRACE);
			break;
		}
		case MODE_FOLLOW_NORMAL_TRACE: ////APPLIES TO: BASIC|ADVANCED|SMART////////////////////////////////////////////////
		{
			/* Check for a white tape on the track */
			if(!change_mode_on_line_detection(sensor_unmsk))
				handle_normal_drive(sensor_unmsk, true, true); /* Drive the car normally */
			break;
		}
		case MODE_AVOID_RIGHT_BOUNDARY: ////APPLIES TO: BASIC|ADVANCED|SMART////////////////////////////////////////////////
		case MODE_AVOID_LEFT_BOUNDARY:
		{
			if(change_mode_on_line_detection(sensor_unmsk))
				break;

			if(track.mode == MODE_AVOID_RIGHT_BOUNDARY && check_right_boundary_recovery(sensor_unmsk))
				change_to_new_mode(MODE_FOLLOW_NORMAL_TRACE, MODE_NULL);
			else if(track.mode == MODE_AVOID_LEFT_BOUNDARY && check_left_boundary_recovery(sensor_unmsk))
				change_to_new_mode(MODE_FOLLOW_NORMAL_TRACE, MODE_NULL);
			break;
		}
		case MODE_FOUND_LEFT_TAPE: ////APPLIES TO: BASIC|ADVANCED|SMART////////////////////////////////////////////////
		case MODE_FOUND_RIGHT_TAPE:
		{
			change_to_new_mode(get_next_turn_mode_from_intel(track.next_turn->is_lane_change), MODE_FOLLOW_NORMAL_TRACE);
			break;
		}
		case MODE_ALIGN_BOUNDARY: ////APPLIES TO: ADVANCED////////////////////////////////////////////////
		{
			/// UNIMPLEMENTED
			break;
		}
		case MODE_TURNING_LANE: ////APPLIES TO: BASIC|ADVANCED////////////////////////////////////////////////
		{
			if(track.intelligence_level == INTEL_BASIC) {
				/* Drive the car slowly until it reaches the other side of the track where the line begins again */

				float limit_speed = track.next_turn->is_lane_change && track.next_turn->direction == TURN_LEFT ? 20 : 40;

				while(READ_LINE(sensor_unmsk)) {
					if(sensor_unmsk == 0) break;
					else pid_update_feedback(pid_controller_current, map_sensor_to_angle(sensor_unmsk), 0);
					motor_ctrl_with_differential2(module_left_wheel, module_right_wheel, limit_speed, pid_controller_current->output);
					rtos_preempt();
				}

				if(track.next_turn->direction == TURN_LEFT) {
					/* Change the lane to the left */
					while(READ_LINE(sensor_unmsk)) {
						if(sensor_unmsk & b11111000) break;
						else pid_update_feedback(pid_controller_current, 0, -15);
						motor_ctrl_with_differential2(module_left_wheel, module_right_wheel, limit_speed, pid_controller_current->output);
						rtos_preempt();
					}

					while(READ_LINE(sensor_unmsk)) {
						if(sensor_unmsk == b00001100) break;
						else pid_update_feedback(pid_controller_current, 0, 10);
						motor_ctrl_with_differential2(module_left_wheel, module_right_wheel, limit_speed, pid_controller_current->output);
						rtos_preempt();
					}
				} else {
					/* Change the lane to the right */
					while(READ_LINE(sensor_unmsk)) {
						if(sensor_unmsk & b00011111) break;
						else pid_update_feedback(pid_controller_current, 0, 10);
						motor_ctrl_with_differential2(module_left_wheel, module_right_wheel, limit_speed, pid_controller_current->output);
						rtos_preempt();
					}

				while(READ_LINE(sensor_unmsk)) {
						if(sensor_unmsk == b00110000) break;
						else pid_update_feedback(pid_controller_current, 0, -10);
						motor_ctrl_with_differential2(module_left_wheel, module_right_wheel, limit_speed, pid_controller_current->output);
						rtos_preempt();
					}
				}

				/* Done lane change */
				track.is_turning_lane = false;
				motor_set_speed(module_left_wheel, module_left_wheel->speed_old);
				motor_set_speed(module_right_wheel, module_right_wheel->speed_old);
				motor_set_braking2(module_left_wheel, module_right_wheel, false);
				pid_reset(pid_controller_current);
				change_to_next_mode(MODE_FOLLOW_NORMAL_TRACE);
				piezo_play(module_piezo, &note_turn_found, true);
			} else { ////ADVANCED MODE
				/// UNIMPLEMENTED
			}
			break;
		}
		case MODE_TURNING_CORNER: ////APPLIES TO: BASIC|ADVANCED////////////////////////////////////////////////
		{
			float limit_speed = 20;

			/* Drive the car through a 90 degree corner (we don't know the direction though, at least in this intel mode) */
			pid_reset(pid_controller_current);

			while(READ_LINE(sensor_unmsk)) {
				if(check_leftline(sensor_unmsk) || sensor_unmsk == b11100000)
					break;
				else if(check_rightline(sensor_unmsk) || sensor_unmsk == b00000111)
					break;
				else
					pid_update_feedback(pid_controller_current, map_sensor_to_angle(sensor_unmsk), 0);
				motor_ctrl_with_differential2(module_left_wheel, module_right_wheel, limit_speed, pid_controller_current->output);
				rtos_preempt();
			}

			if(track.next_turn->direction == TURN_RIGHT) {
				while(READ_LINE(sensor_unmsk)) {
					if(sensor_unmsk == b00011000) break;
					else pid_update_feedback(pid_controller_current, 0, 90);
					motor_ctrl(module_left_wheel, module_left_wheel->max_speed);
					motor_ctrl(module_right_wheel, 0);
					rtos_preempt();
				}
			} else {
				while(READ_LINE(sensor_unmsk)) {
					if(sensor_unmsk == b01100000) break;
					else pid_update_feedback(pid_controller_current, 0, -90);
					motor_ctrl(module_right_wheel, module_right_wheel->max_speed);
					motor_ctrl(module_left_wheel, 0);
					rtos_preempt();
				}
			}

			/* Done 90 deg corner */
			track.is_turning_corner = false;
			motor_set_speed(module_left_wheel, module_left_wheel->speed_old);
			motor_set_speed(module_right_wheel, module_right_wheel->speed_old);
			motor_set_braking2(module_left_wheel, module_right_wheel, false);
			pid_reset(pid_controller_current);
			change_to_next_mode(MODE_FOLLOW_NORMAL_TRACE);
			piezo_play(module_piezo, &note_turn_found, true);

			break;
		}
		case MODE_TURNING_CORNER_BLIND: ////APPLIES TO: SMART////////////////////////////////////////////////
		{
			/* Drive the car blindly using the template sensor data */
			bool motors_updated;
			uint8_t sensor_data = ltracker_read(MASK4_4, &motors_updated);

			/* Keep driving until we reach the end of the 'blind' data set */
			handle_normal_drive(sensor_data, false, !motors_updated);
			break;
		}
	}
}
///// END - MAIN CONTROL FINITE STATE MACHINE /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

