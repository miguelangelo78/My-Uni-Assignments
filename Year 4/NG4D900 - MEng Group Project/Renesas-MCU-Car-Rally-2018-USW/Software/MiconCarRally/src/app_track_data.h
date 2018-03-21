/*
 * app_data.h
 *
 *  Created on: 27/11/2017
 *      Author: Miguel
 */

#ifndef SRC_APP_TRACK_DATA_H_
#define SRC_APP_TRACK_DATA_H_

#include <utils.h>
#include <app_config.h>

track_t track = {
	.intelligence_level           = { INTEL_BASIC               },
	.mode                         = { MODE_WAIT_FOR_STARTSWITCH },
	.next_mode                    = { MODE_FOLLOW_NORMAL_TRACE  },
	.next_turn                    = { NULL  },
	.servo_override               = { false },
	.momentum_map_triggered       = { false },
	.momentum_counter             = { 0 },
	.line_misread_danger_counter  = { 0 },
	.momentum_map_trigger_counter = { 0 },

	/* What patterns shall we match and how should we react to them */
	.pattern_map = {
		{b00011000,  0,  100, 100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,   1 },
		{b00010000,  0,  100, 100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,   1 },
		{b00001000,  0,  100, 100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,   1 },
		{b00100000,  29, 65,  65,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b01000000,  45, 70,  70,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b00000010, -50, 70,  70,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b00000100, -29, 65,  65,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b00011100, -14, 65,  65,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b00001100, -18, 65,  65,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b00001110, -29, 70,  70,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b00000110, -40, 65,  65,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b00000111, -50, 75,  75,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b00000011, -55, 65,  65,  5, 0.1, 100000, MODE_AVOID_RIGHT_BOUNDARY, -1 },
		{b00111000,  7,  65,  65,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b00110000,  19, 65,  65,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b01110000,  29, 70,  70,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b01100000,  40, 65,  65,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b11100000,  45, 75,  75,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE,  -1 },
		{b11000000,  55, 65,  65,  5, 0.1, 100000, MODE_AVOID_LEFT_BOUNDARY,  -1 }
	},

	/* Which turn will we encounter next */
	.incoming_turn = {
#if TRACK_TYPE == 0
		/* 1st turn: lane change to the right */
		{
			.is_lane_change = true,
			.direction = TURN_RIGHT,
		},

		/* 2nd turn: lane change to the left */
		{
			.is_lane_change = true,
			.direction = TURN_LEFT,
		},


		/* 3rd turn: 90 deg corner to the right */
		{
			.is_lane_change = false,
			.direction = TURN_RIGHT,
		},

		/* 4th turn: 90 deg corner to the right */
		{
			.is_lane_change = false,
			.direction = TURN_RIGHT,
		},

		/* 5th turn: 90 deg corner to the left */
		{
			.is_lane_change = false,
			.direction = TURN_LEFT,
		},

		/* 6th turn: 90 deg corner to the left */
		{
			.is_lane_change = false,
			.direction = TURN_LEFT,
		},

		/* 7th turn: 90 deg corner to the left */
		{
			.is_lane_change = false,
			.direction = TURN_LEFT,
		},

		/* 8th turn: 90 deg corner to the right */
		{
			.is_lane_change = false,
			.direction = TURN_RIGHT,
		},
#elif TRACK_TYPE == 1
		/* 1st turn: lane change to the right */
		{
			.is_lane_change = true,
			.direction = TURN_RIGHT,
		},

		/* 2nd turn: lane change to the left */
		{
			.is_lane_change = true,
			.direction = TURN_LEFT,
		},


		/* 3rd turn: 90 deg corner to the right */
		{
			.is_lane_change = false,
			.direction = TURN_RIGHT,
		},

		/* 4th turn: 90 deg corner to the left */
		{
			.is_lane_change = false,
			.direction = TURN_LEFT,
		},

		/* 5th turn: 90 deg corner to the left */
		{
			.is_lane_change = false,
			.direction = TURN_LEFT,
		}
#elif TRACK_TYPE == 2
		/* 1st turn: lane change to the left */
		{
			.is_lane_change       = true,
			.direction            = TURN_LEFT,
			.corner_fetch_pattern = 0
		},

		/* 2nd turn: lane change to the right */
		{
			.is_lane_change       = true,
			.direction            = TURN_RIGHT,
			.corner_fetch_pattern = 0
		},

		/* 3rd turn: 90 deg corner to the right */
		{
			.is_lane_change       = false,
			.direction            = TURN_RIGHT,
			.corner_fetch_pattern = b00011000,
			.corner_brake_speed   = 5
		},

		/* 4th turn: 90 deg corner to the left */
		{
			.is_lane_change       = false,
			.direction            = TURN_LEFT,
			.corner_fetch_pattern = b00011000,
			.corner_brake_speed   = BRAKE_CORNER_SPEED_PERCENTAGE
		},

		/* 5th turn: 90 deg corner to the left */
		{
			.is_lane_change       = false,
			.direction            = TURN_LEFT,
			.corner_fetch_pattern = b00011000,
			.corner_brake_speed   = -20
		},

		/* 6th turn: 90 deg corner to the left */
		{
			.is_lane_change       = false,
			.direction            = TURN_LEFT,
			.corner_fetch_pattern = b00011000,
			.corner_brake_speed   = BRAKE_CORNER_SPEED_PERCENTAGE
		},

		/* 7th turn: 90 deg corner to the left */
		{
			.is_lane_change       = false,
			.direction            = TURN_LEFT,
			.corner_fetch_pattern = b00011000,
			.corner_brake_speed   = 15
		}
#elif TRACK_TYPE == 3
		/* 1st turn: 90 deg corner to the left */
		{
			.is_lane_change = false,
			.direction = TURN_LEFT,
			.corner_fetch_pattern = b00011000,
			.corner_brake_speed = 0
		},

		/* 2nd turn: 90 deg corner to the left */
		{
			.is_lane_change = false,
			.direction = TURN_LEFT,
			.corner_fetch_pattern = b00011000,
			.corner_brake_speed = 0
		},

		/* 3rd turn: 90 deg corner to the left */
		{
			.is_lane_change = false,
			.direction = TURN_LEFT,
			.corner_fetch_pattern = b00011000,
			.corner_brake_speed = -15
		},

		/* 4th turn: 90 deg corner to the left */
		{
			.is_lane_change = false,
			.direction = TURN_LEFT,
			.corner_fetch_pattern = b00011000,
			.corner_brake_speed = 0
		}
#endif
	},
};

char * mode_string_list[] = {
	"MODE_NULL",
	"MODE_WAIT_FOR_STARTSWITCH",
	"MODE_FOLLOW_NORMAL_TRACE",
	"MODE_AVOID_RIGHT_BOUNDARY",
	"MODE_AVOID_LEFT_BOUNDARY",
	"MODE_FOUND_LEFT_TAPE",
	"MODE_FOUND_RIGHT_TAPE",
	"MODE_ALIGN_BOUNDARY",
	"MODE_TURNING_LANE",
	"MODE_TURNING_CORNER",
	"MODE_TURNING_CORNER_BLIND",
	"MODE_ACCIDENT",
	"MODE_RACE_COMPLETE",
	"MODE_REMOTE"
};

char * intel_string_list[] = {
	"INTEL_BASIC",
	"INTEL_ADVANCED",
	"INTEL_SMART"
};

#endif /* SRC_APP_TRACK_DATA_H_ */
