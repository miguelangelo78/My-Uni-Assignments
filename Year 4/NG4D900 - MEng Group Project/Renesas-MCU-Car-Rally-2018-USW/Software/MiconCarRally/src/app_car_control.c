/*
 * app_car_control.c
 *
 *  Created on: 20/02/2018
 *      Author: Miguel
 */

#include <app_car_control.h>

motor_t * module_left_wheel  = NULL; /* Left  DC Motor module */
motor_t * module_right_wheel = NULL; /* Right DC Motor module */
servo_t * module_servo       = NULL; /* Servo module          */

pid_t * pid_controller_normal    = NULL; /* PID for controlling the servo angle and the DC motor differential */
pid_t * pid_controller_crankmode = NULL; /* PID for controlling each DC motor while in brake mode             */
pid_t * pid_controller_current   = NULL; /* What current PID controller are we using at any given time        */

float pid_output = 0.0f; /* The final result of the PID that is ready to be used by the servo and DC motors */

piezo_t * module_piezo = NULL; /* Piezo buzzer module */

///////////////////////////////////////////////////////////////////////////////
void update_track_status(void) {
	/* Fetch the next incoming turn */
	if(track.next_turn == NULL) {
		track.next_turn = &track.incoming_turn[0];
	} else {
		if(++track.turn_counter >= LAP_MAX_TURNS) {
			/* We have completed a lap */
			track.turn_counter = 1; /* Counter does not reset back to 0 because the first and last turns STORED IN MEMORY are meant for the same (physical) turns */

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
enum MODE get_next_turn_mode_from_intel(bool is_next_turn_a_lane_change) {
	update_track_status();

	if(is_next_turn_a_lane_change) {
		track.is_turning_lane = true;
		switch(track.intelligence_level) {
		case INTEL_BASIC:    return MODE_TURNING_LANE;
		case INTEL_ADVANCED: return MODE_ALIGN_BOUNDARY;
		case INTEL_SMART:    return MODE_TURNING_CORNER_BLIND;
		default:             return MODE_NULL;
		}
	} else {
		track.is_turning_corner = true;
		switch(track.intelligence_level) {
		case INTEL_BASIC:    return MODE_TURNING_CORNER;
		case INTEL_ADVANCED: return MODE_ALIGN_BOUNDARY;
		case INTEL_SMART:    return MODE_TURNING_CORNER_BLIND;
		default:             return MODE_NULL;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////
bool change_mode_on_line_detection(uint8_t sensor_data) {
	bool ret = false;

	/* Check crossline tape which covers both lanes */
	if((ret = check_crossline(sensor_data))) {
#if ENABLE_TEMPLATE_GENERATION == 1
		template_generator_begin();
#else
		if(ENABLE_DIPSWITCH && is_in_template_generation_mode())
			template_generator_begin();
		else
			change_to_new_mode(get_next_turn_mode_from_intel(true), MODE_FOLLOW_NORMAL_TRACE);
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
		piezo_play(module_piezo, &note_alert, false);

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

///////////////////////////////////////////////////////////////////////////////
void car_control_poll(void) {
	if(module_left_wheel == NULL || module_right_wheel == NULL || module_servo == NULL)
		return;

	/* We have finished the race. Do not update the controls anymore. */
	if(track.mode == MODE_RACE_COMPLETE)
		return;

	/* Read the infrared sensor with no mask */
	uint8_t sensor_unmsk = ltracker_read(MASK4_4, NULL);

	/* Check for off-track accidents */
	if(rtos_time() % 50 == 0) {
		if(!track.is_turning_lane && (sensor_unmsk == b11111111 || sensor_unmsk == b00000000)) {
			if(++track.line_misread_danger_counter >= 5) {
				/* Completely stop the car and center the servo if all of the sensors have been on/off for this period of time */
				motor_ctrl2(module_left_wheel, module_right_wheel, 0);
				servo_ctrl(module_servo, 0);
				change_to_next_mode(MODE_ACCIDENT);

				/* Alert the user of the event */
				piezo_play(module_piezo, &note_alert, false);
			}
		} else {
			track.line_misread_danger_counter = 0;
		}
	}

	///// MAIN CONTROL FINITE STATE MACHINE ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	switch(track.mode) {
		case MODE_NULL: /* The car shouldn't react to this */ return;
		case MODE_WAIT_FOR_STARTSWITCH: ////APPLIES TO: BASIC|ADVANCED|SMART////////////////////////////////////////////////
		{
			/* Wait until the user presses the start button.
			   Meanwhile, we'll lock the servo onto the line */
			handle_normal_drive(sensor_unmsk, false, false);
			break;
		}
		case MODE_ACCIDENT:
		{
			/* Block the control system from updating its variables */
			while(!start_switch_read()) rtos_update_timeout_service(); /* Still need to let RTOS work properly for other tasks even though we're blocking this thread */
			/* (The car better be back on the track when we reach this point...) */
			change_to_next_mode(MODE_FOLLOW_NORMAL_TRACE);
			break;
		}
		case MODE_FOLLOW_NORMAL_TRACE: ////APPLIES TO: BASIC|ADVANCED|SMART////////////////////////////////////////////////
		{
			/* Check for a white tape on the track */
			if(change_mode_on_line_detection(sensor_unmsk))
				break;

			/* Drive the car normally */
			handle_normal_drive(sensor_unmsk, true, true);
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
			/* Change lane to the left / right if and ONLY if we did NOT "misdetect" a crossline */
			change_to_new_mode(get_next_turn_mode_from_intel(sensor_unmsk & MASK3_3 != b11100111), MODE_FOLLOW_NORMAL_TRACE);
			break;
		}
		case MODE_ALIGN_BOUNDARY: ////APPLIES TO: ADVANCED////////////////////////////////////////////////
		{

			break;
		}
		case MODE_TURNING_LANE: ////APPLIES TO: BASIC|ADVANCED////////////////////////////////////////////////
		{
			if(track.intelligence_level == INTEL_BASIC) {
				/* Drive the car slowly until it reaches the other side of the track where the line begins again */
				if(track.last_mode == MODE_FOUND_LEFT_TAPE) {
					/* Change the lane to the left */
					// TODO (don't forget to set track.is_turning_lane = false and braking = false after done)
				} else if(track.last_mode == MODE_FOUND_RIGHT_TAPE) {
					/* Change the lane to the right */
					// TODO (don't forget to set track.is_turning_lane = false and braking = false after done)
				}
			} else { ////ADVANCED MODE

			}
			break;
		}
		case MODE_TURNING_CORNER: ////APPLIES TO: BASIC|ADVANCED////////////////////////////////////////////////
		{
			/* Drive the car through a 90 degree corner (we don't know the direction though, at least in this intel mode) */
			// TODO (don't forget to set track.is_turning_corner = false and braking = false after done)
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
	///// END - MAIN CONTROL FINITE STATE MACHINE /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	/* Switch to the correct PID controller, which depends on whether or not we are breaking/slowing down */
	pid_controller_current = (module_left_wheel->is_braking || module_right_wheel->is_braking) ? pid_controller_crankmode : pid_controller_normal;

	/* Recalculate the PID controller values */
	pid_output = pid_control(pid_controller_current);

	/* And update the external systems respectively */
	if(!module_servo->is_sweeping) {
		/* Update the servo's duty cycle */
		servo_ctrl(module_servo, pid_output);

		/* And the motors (after the user presses the switch) */
		if(track.mode > MODE_WAIT_FOR_STARTSWITCH)
			motor_refresh_with_differential2(module_left_wheel, module_right_wheel, pid_output);
	} else {
		/* Stop the car while the servo is sweeping */
		motor_set_speed2(module_left_wheel, module_right_wheel, 0);
	}
}
