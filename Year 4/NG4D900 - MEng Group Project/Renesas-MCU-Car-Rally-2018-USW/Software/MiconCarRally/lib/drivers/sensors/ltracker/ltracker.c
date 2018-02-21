/*
 * ltracker.c
 *
 *  Created on: 27/11/2017
 *      Author: Miguel
 */

#include <rtos_inc.h>
#include <app_config.h>
#include <app_car_control.h>
#include <actuators/motor_driver/motor_driver.h>
#include "ltracker.h"

extern track_t   track;
extern motor_t * module_left_wheel;
extern motor_t * module_right_wheel;

uint8_t ltracker_read(uint8_t mask, bool * motors_updated) {
	if(track.mode == MODE_TURNING_CORNER_BLIND) { //// SMART MODE
		turn_t * next_turn = track.next_turn;

		uint16_t fake_sample_count = (next_turn->fake_line_data_max_count > 0 ? next_turn->fake_line_data_max_count : FAKE_LINEDATA_MAX_SAMPLES) - 1;
		if(fake_sample_count >= FAKE_LINEDATA_MAX_SAMPLES)
			fake_sample_count = FAKE_LINEDATA_MAX_SAMPLES - 1;

		fake_line_data_t * current_fake_line = &next_turn->fake_line_data[next_turn->fake_line_data_index];

		if(rtos_get_timeout(false, false, current_fake_line->correction_period, 0))
			current_fake_line = &next_turn->fake_line_data[++next_turn->fake_line_data_index];

		if(next_turn->fake_line_data_index + 1 > fake_sample_count) {
			/***********************************/
			/** And we're done with the turn! **/
			/***********************************/

			/* Stop braking the motors */
			motor_set_braking2(module_left_wheel, module_right_wheel, false);

			/* Set the mode to follow the track (normal mode) */
			change_to_next_mode(MODE_FOLLOW_NORMAL_TRACE);

			next_turn->fake_line_data_index = 0;

			if(motors_updated != NULL)
				*motors_updated = false;
		} else {
			if(current_fake_line->left_motor_brake_speed > 0.0)
				motor_set_speed(module_left_wheel, current_fake_line->left_motor_brake_speed);
			if(current_fake_line->right_motor_brake_speed > 0.0)
				motor_set_speed(module_right_wheel, current_fake_line->right_motor_brake_speed);

			if(motors_updated != NULL)
				*motors_updated = true;
		}

		/* Return the fake sensor data */
		return current_fake_line->error_pattern & mask;
	}

	return (~INP_LTRACKER) & mask;
}
