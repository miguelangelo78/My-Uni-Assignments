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
	.intelligence_level = { INTEL_BASIC },
	.mode               = { MODE_WAIT_FOR_STARTSWITCH },
	.next_mode          = { MODE_FOLLOW_NORMAL_TRACE  },
	.line_misread_danger_counter = { 0 },

	/* What patterns shall we match and how should we react to them */
	.pattern_map = {
		{b00011000,  0,  100, 100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b00010000,  0,  100, 100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b00001000,  0,  100, 100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b00100000,  7,  100, 100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b01000000,  10, 95,  100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b10000000,  10, 92,  100, 5, 0.1, 100000, MODE_AVOID_LEFT_BOUNDARY },
		{b00000001,  10, 100, 85,  5, 0.1, 100000, MODE_AVOID_RIGHT_BOUNDARY},
		{b00000010, -10, 100, 90,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b00000100, -7,  100, 95,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b00011100, -5,  100, 100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b00001100, -7,  100, 97,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b00001110, -10, 100, 90,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b00000110, -15, 100, 85,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b00000111, -25, 100, 85,  5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b00000011, -30, 100, 80,  5, 0.1, 100000, MODE_AVOID_RIGHT_BOUNDARY},
		{b00111000,  5,  100, 100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b00110000,  7,  100, 100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b01110000,  10, 95,  100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b01100000,  15, 90,  100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b11100000,  25, 90,  100, 5, 0.1, 100000, MODE_FOLLOW_NORMAL_TRACE },
		{b11000000,  30, 85,  100, 5, 0.1, 100000, MODE_AVOID_LEFT_BOUNDARY }
	},

	/* Which turn will we encounter next */
	.incoming_turn = {
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
