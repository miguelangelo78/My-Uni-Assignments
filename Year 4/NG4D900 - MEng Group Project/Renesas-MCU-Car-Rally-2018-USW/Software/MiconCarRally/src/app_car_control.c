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
piezo_t * module_piezo       = NULL; /* Piezo buzzer module   */

pid_t * pid_controller = NULL; /* PID for controlling the servo angle and the DC motor differential */

///////////////////////////////////////////////////////////////////////////////
uint8_t car_update_control(void)
{
	/* Read all 8 infra-red sensors at the front */
	uint8_t line_sensor = ltracker_read(MASK4_4, NULL);

	int pid_output = 0;

#if ENABLE_PID == 1
	/* Update PID feedback before recalculation */
	pid_update_feedback(pid_controller, map_sensor_to_angle(line_sensor), 0);

	/* Recalculate PID with new feedback */
	pid_output = pid_control_recalculate(pid_controller);
#endif

#if ENABLE_SERVO == 1 && ENABLE_MOTORS == 1
	/* Now update the external systems according to the PID output */
	if(!module_servo->is_sweeping) {
		/** 1- Update the servo's duty cycle (UPDATE RATE: 20 MS / 50 HZ) **/
		if(!track.timeout_disable_control && !track.servo_override && rtos_time() % 20 == 0)
			servo_ctrl(module_servo, pid_output);

		/** 2- Update the motors' differential (UPDATE RATE: 0.02213 ms (~22.13 us) / ~45.18 kHz) **/
		if(track.mode > MODE_WAIT_FOR_STARTSWITCH && !track.timeout_disable_control) {
			if(!module_left_wheel->is_braking && !module_right_wheel->is_braking) {
				motor_refresh_with_differential(module_left_wheel , (float)pid_output);
				motor_refresh_with_differential(module_right_wheel, (float)pid_output);
			}
		} else {
			/* Stop the car if we're not racing */
			motor_ctrl2(module_left_wheel, module_right_wheel, 0);
		}
	} else {
		/* Stop the car while the servo is sweeping */
		motor_ctrl2(module_left_wheel, module_right_wheel, 0);
	}
#endif

	return line_sensor;
}

///////////////////////////////////////////////////////////////////////////////
void update_track_status(void)
{
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
				motor_set_braking2(module_left_wheel, module_right_wheel, true);
				motor_set_speed2(module_left_wheel, module_right_wheel, 0);

				/* TODO: Play song signaling the end of the race */
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
		if(ENABLE_DIPSWITCH && is_in_template_generation_mode())
			template_generator_begin();
		else
#endif
			change_to_new_mode(get_next_turn_mode_from_intel(false), MODE_FOLLOW_NORMAL_TRACE);
	}

	/* Check white tape which covers only the right lane */
	if(!ret && (ret = check_rightline(sensor_data))) {
#if ENABLE_TEMPLATE_GENERATION == 1
		if(ENABLE_DIPSWITCH && is_in_template_generation_mode())
			template_generator_begin();
		else
#endif
			change_to_new_mode(MODE_FOUND_RIGHT_TAPE, MODE_NULL);
	}

	/* Check white tape which covers only the left lane */
	if(!ret && (ret = check_leftline(sensor_data))) {
#if ENABLE_TEMPLATE_GENERATION == 1
		if(ENABLE_DIPSWITCH && is_in_template_generation_mode())
			template_generator_begin();
		else
#endif
			change_to_new_mode(MODE_FOUND_LEFT_TAPE, MODE_NULL);
	}

	if(ret) {
		/* We found a white tape. Don't care which one for now,
		 * all we know now is that we must slow down the wheels */
		piezo_play(module_piezo, &note_turn_found, false);
		update_track_status();

		if(track.intelligence_level != INTEL_SMART) {
			motor_set_speed(module_left_wheel,  module_left_wheel->speed  * 0.7);
			motor_set_speed(module_right_wheel, module_right_wheel->speed * 0.7);
		}
	}

#if ENABLE_TEMPLATE_GENERATION == 1
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
#if ENABLE_DYNAMIC_PID == 1
				/* Update the PID coefficients for every different pattern */
				pid_change_constants(pid_controller, track.pattern_map[i].p, track.pattern_map[i].i, track.pattern_map[i].d);
#endif

#if ENABLE_DYNAMIC_MOMENTUM == 1
				static float motor_left_old_latched_speed  = 0.0f;
				static float motor_right_old_latched_speed = 0.0f;

				if(track.momentum_map_triggered && !track.servo_override) {
					motor_set_speed(module_left_wheel,  motor_left_old_latched_speed  * 0.7);
					motor_set_speed(module_right_wheel, motor_right_old_latched_speed * 0.7);
				} else {
					motor_set_speed(module_left_wheel,  track.pattern_map[i].desired_motor_left);
					motor_set_speed(module_right_wheel, track.pattern_map[i].desired_motor_right);
				}

				if(!track.momentum_map_triggered) {
					if(track.momentum_counter >= MOMENTUM_EASE_ENABLE_THRESHOLD) {
						motor_left_old_latched_speed  = module_left_wheel->speed;
						motor_right_old_latched_speed = module_right_wheel->speed;
						track.momentum_map_triggered  = true;
						track.momentum_map_trigger_counter++;
					}
				} else {
					if(track.momentum_counter <= MOMENTUM_EASE_DISABLE_THRESHOLD)
						track.momentum_map_triggered = false;
				}

				if(!track.servo_override && track.race_started) {
					track.momentum_counter += track.pattern_map[i].delta_momentum;
					if(track.momentum_counter <= 0)
						track.momentum_counter = 0;
					if(track.momentum_counter >= MOMENTUM_EASE_ENABLE_THRESHOLD * 4)
						track.momentum_counter = MOMENTUM_EASE_ENABLE_THRESHOLD * 4;
				} else {
					track.momentum_counter       = 0;
					track.momentum_map_triggered = false;
				}

#else
				motor_set_speed2(module_left_wheel, module_right_wheel, 100.0f);
#endif
			}
			return track.pattern_map[i].mapped_angle;
		}

	log_unrecognized_pattern(sensor_data);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
void car_kickstart(void) {
	if(track.mode == MODE_WAIT_FOR_STARTSWITCH) {
		/* Kick start the car! */
		spwm_set_frequency(module_left_wheel->dev_handle,  MOTOR_FREQ);
		spwm_set_frequency(module_right_wheel->dev_handle, MOTOR_FREQ);

		change_to_new_mode(MODE_FOLLOW_NORMAL_TRACE, MODE_FOLLOW_NORMAL_TRACE);
		motor_set_braking2(module_left_wheel, module_right_wheel, false);

		track.race_started = true;

		/* Alert the user of the event */
		piezo_play(module_piezo, &note_startswitch, false);

		DEBUG(">> GO <<");
	}
}

///////////////////////////////////////////////////////////////////////////////
void car_algorithm_poll(uint8_t line_sensor)
{
	/* We have finished the race. Do not update the logic anymore */
	if(track.mode == MODE_RACE_COMPLETE)
		return;

	/* Check for off-track accidents every 50 ms */
	if(rtos_time() % 50 == 0) {
		if(!track.is_turning_lane && !track.is_turning_corner && track.mode > MODE_WAIT_FOR_STARTSWITCH &&
			(line_sensor == b11111111 || line_sensor == b00000000))
		{
			if(++track.line_misread_danger_counter >= 100)
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
			break;
		}
		case MODE_ACCIDENT:
		{
			static bool module_piezo_playing_alert = false;

			/* Completely stop the car and center the servo if all of the
			 * sensors have been on/off for a certain amount of time */
			motor_stop2(module_left_wheel, module_right_wheel);
			motor_set_braking2(module_left_wheel, module_right_wheel, false);
			servo_ctrl(module_servo, 0);

			if(start_switch_read()) {
				/* (The car better be back on the track when we reach this point...) */
				change_to_next_mode(MODE_FOLLOW_NORMAL_TRACE);
				piezo_stop(module_piezo, false);
				module_piezo_playing_alert = false;
			} else {
				if(module_piezo->is_playing && !module_piezo_playing_alert) {
					/* Stop the control system from updating its variables and alert the user of the event */
					piezo_stop(module_piezo, false);
				} else {
					if(module_piezo_playing_alert) {
						if(!module_piezo->is_playing && rtos_time() % 150 == 0)
							module_piezo_playing_alert = false;
					} else {
						module_piezo_playing_alert = true;
						piezo_play(module_piezo, &note_alert, false);
					}
				}
			}

			break;
		}
		case MODE_FOLLOW_NORMAL_TRACE: ////APPLIES TO: BASIC|ADVANCED|SMART////////////////////////////////////////////////
		{
			/* Check for a white tape on the track */
			if(!change_mode_on_line_detection(line_sensor)) {
				/* Update the state based on the pattern map */
				for(int i = 0; i < PATTERN_MAP_SIZE; i++) {
					if(line_sensor == track.pattern_map[i].pattern) {
						change_to_new_mode(track.pattern_map[i].change_mode, MODE_NULL);
						break;
					}
				}
			}
			break;
		}
		case MODE_AVOID_RIGHT_BOUNDARY: ////APPLIES TO: BASIC|ADVANCED|SMART////////////////////////////////////////////////
		case MODE_AVOID_LEFT_BOUNDARY:
		{
			if(change_mode_on_line_detection(line_sensor))
				break;

			if(track.mode == MODE_AVOID_RIGHT_BOUNDARY && check_right_boundary_recovery(line_sensor))
				change_to_new_mode(MODE_FOLLOW_NORMAL_TRACE, MODE_NULL);
			else if(track.mode == MODE_AVOID_LEFT_BOUNDARY && check_left_boundary_recovery(line_sensor))
				change_to_new_mode(MODE_FOLLOW_NORMAL_TRACE, MODE_NULL);
			break;
		}
		case MODE_FOUND_LEFT_TAPE: ////APPLIES TO: BASIC|ADVANCED|SMART////////////////////////////////////////////////
		case MODE_FOUND_RIGHT_TAPE:
		{
			if(track.intelligence_level != INTEL_SMART)
				motor_set_braking2(module_left_wheel, module_right_wheel, true);
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
				static uint8_t turn_lane_state = 0;
				float limit_speed = track.next_turn->corner_fetch_pattern == 0 ? (track.laps_completed == 0 ? BRAKE_LANE_CHANGE_NOMOMENTUM : BRAKE_LANE_CHANGE_WITHMOMENTUM) : track.next_turn->corner_brake_speed;

				switch(turn_lane_state) {
					case 0: /* Wait until we reach the 'line break' */
						if(line_sensor == 0) {
							track.servo_override = true;
							turn_lane_state++;
						}
						else pid_update_feedback(pid_controller, map_sensor_to_angle(line_sensor), 0);
						motor_ctrl_with_differential2(module_left_wheel, module_right_wheel, limit_speed, pid_controller->output);
						track.is_turning_lane   = true;
						track.is_turning_corner = false;
						break;
					case 1: /* Change the lane to the left/right */
						if(line_sensor & (track.next_turn->direction == TURN_LEFT ? b11111000 : b00011111))
						{
							motor_set_speed(module_left_wheel,    module_left_wheel->speed_old);
							motor_set_speed(module_right_wheel,   module_right_wheel->speed_old);
							motor_set_braking2(module_left_wheel, module_right_wheel, false);
							track.servo_override = false;
							turn_lane_state++;
						} else {
							servo_ctrl(module_servo, track.next_turn->direction == TURN_LEFT ? -45 : 45);
						}

						motor_ctrl_with_differential2(module_left_wheel, module_right_wheel, limit_speed, pid_controller->output);
						break;
					case 2: /* Try to catch the beginning of the line on the other side */
						if(line_sensor == /* Match the sensor data with the conditions below */
							(track.next_turn->corner_fetch_pattern == 0 ? (track.next_turn->direction == TURN_LEFT ? b00001100 : b00110000) :
							(track.next_turn->corner_fetch_pattern)))
						{
							turn_lane_state++;
						}
						break;
					case 3:
						/* Done lane change */
						track.is_turning_lane  = false;
						turn_lane_state        = 0;
						change_to_next_mode(MODE_FOLLOW_NORMAL_TRACE);
						piezo_play(module_piezo, &note_turn_found, false);
						break;
				}
			} else { ////ADVANCED MODE
				/// UNIMPLEMENTED
			}
			break;
		}
		case MODE_TURNING_CORNER: ////APPLIES TO: BASIC|ADVANCED////////////////////////////////////////////////
		{
			static uint8_t turn_corner_state = 0;

			float limit_speed = track.next_turn->corner_brake_speed;

			/* Drive the car through a 90 degree corner (we don't know the direction though, at least in this intel mode) */

			switch(turn_corner_state) {
				case 0: /* Drive slowly until we reach the corner */
					if(line_sensor == 0) {
#if FLOOR_IS_ON == 0
						turn_corner_state++;
#else
						turn_corner_state += 3;
#endif
						track.servo_override = true;
						if(track.next_turn->direction == TURN_RIGHT) {
							servo_ctrl(module_servo, 90);
							motor_ctrl(module_left_wheel, module_left_wheel->max_speed * 0.6);
							motor_ctrl(module_right_wheel, 0);
						} else {
							servo_ctrl(module_servo, -90);
							motor_ctrl(module_left_wheel, 0);
							motor_ctrl(module_right_wheel, module_right_wheel->max_speed * 0.6);
						}
						break;
					} else {
						pid_update_feedback(pid_controller, map_sensor_to_angle(line_sensor), 0);
					}

					motor_ctrl_with_differential2(module_left_wheel, module_right_wheel, limit_speed, pid_controller->output);
					track.is_turning_lane   = false;
					track.is_turning_corner = true;
					break;
				case 1: /* Keep turning and check if we went too far on the corner */
					if(line_sensor & (track.next_turn->direction == TURN_RIGHT ? b01100000 : b00000110))
						turn_corner_state++;
					else if(line_sensor == track.next_turn->corner_fetch_pattern)
						turn_corner_state += 3; /* We did not go beyond the corner and thus we did not reach the corner's border. */
					break;
				case 2: /* We have gone beyond the corner, but we're stil on the track. Keep turning and check for the pattern below */
					if(line_sensor == b10000001)
						turn_corner_state++;
					break;
				case 3: /* If we get here then we've gone beyond the corner. It is now time to recover and we'll match the correct pattern */
					if(line_sensor == track.next_turn->corner_fetch_pattern)
						turn_corner_state++;
					break;
				case 4: /* Finished the 90 degree corner */
					track.is_turning_corner = false;
					track.servo_override    = false;
					change_to_next_mode(MODE_FOLLOW_NORMAL_TRACE);
					motor_set_braking2(module_left_wheel, module_right_wheel, false);
					motor_set_speed(module_left_wheel,  module_left_wheel->speed_old);
					motor_set_speed(module_right_wheel, module_right_wheel->speed_old);
					piezo_play(module_piezo, &note_turn_found, false);
					turn_corner_state = 0;
					break;
				}

			break;
		}
		case MODE_TURNING_CORNER_BLIND: ////APPLIES TO: SMART////////////////////////////////////////////////
		{
			/* Drive the car blindly using the template sensor data */
			bool motors_updated;
			uint8_t sensor_data = ltracker_read(MASK4_4, &motors_updated);

			/* Keep driving until we reach the end of the 'blind' data set */
			// xxx UNIMPLEMENTED xxx handle_normal_drive(sensor_data, false, !motors_updated); xxx UNIMPLEMENTED xxx
			break;
		}
	}
}
///// END - MAIN CONTROL FINITE STATE MACHINE /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
