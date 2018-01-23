/*
 * app_data.h
 *
 *  Created on: 27/11/2017
 *      Author: Miguel
 */

#ifndef SRC_APP_TRACK_DATA_H_
#define SRC_APP_TRACK_DATA_H_

#include <utils.h>
#include "app_config.h"

track_t track = {
	.mode      = { MODE_WAIT_FOR_STARTSWITCH },
	.next_mode = { MODE_FOLLOW_NORMAL_TRACE  },

	/* What patterns shall we match and how should we react to them */
	.pattern_map = {
		{b00011000,  0 , 100, 100, MODE_FOLLOW_NORMAL_TRACE  },
		{b00011100,  5 , 95,  95,  MODE_FOLLOW_NORMAL_TRACE  },
		{b00001100,  6 , 90,  90,  MODE_FOLLOW_NORMAL_TRACE  },
		{b00001110,  7,  90,  90,  MODE_FOLLOW_NORMAL_TRACE  },
		{b00000110,  10, 87,  87,  MODE_FOLLOW_NORMAL_TRACE  },
		{b00000111,  15, 87,  87,  MODE_FOLLOW_NORMAL_TRACE  },
		{b00000011,  17, 87,  87,  MODE_AVOID_RIGHT_BOUNDARY },
		{b00111000, -5,  95,  95,  MODE_FOLLOW_NORMAL_TRACE  },
		{b00110000, -6,  90,  90,  MODE_FOLLOW_NORMAL_TRACE  },
		{b01110000, -7,  90,  90,  MODE_FOLLOW_NORMAL_TRACE  },
		{b01100000, -10, 87,  87,  MODE_FOLLOW_NORMAL_TRACE  },
		{b11100000, -15, 87,  87,  MODE_FOLLOW_NORMAL_TRACE  },
		{b11000000, -17, 87,  87,  MODE_AVOID_LEFT_BOUNDARY  },
		{b00000000,  0,  0,   0,   MODE_WAIT_FOR_STARTSWITCH },
		{b11111111,  0,  0,   0,   MODE_WAIT_FOR_STARTSWITCH },
		/* TODO: IMPLEMENT LINEAR INTERPOLATION TO PREVENT USING THE PATTERN MAP ARRAY ABOVE */
	},

	/* Which turn will we encounter next */
	.incoming_turn = {
		/* 1st turn */
		{TURN_LEFT, false,
			/* 90 degree corner to the left */
			.fake_line_data_max_count = 9,
			.fake_line_data = {
				{b00011000, 10 * 20},
				{b00011000, 10 * 20},
				{b00011000, 10 * 10},
				{b00011000, 3},
				{b11100000, 10 * 20},
				{b11100000, 10 * 10},
				{b11100000, 10 * 10},
				{b11100000, 10 * 10},
				{b00011000, 10 * 10}
			},
		},

		/* 2nd turn */
		/* TODO ... */

		/* 1st turn again (in fast mode) */
		{TURN_LEFT, false,
			/* 90 degree corner to the left */
			.fake_line_data_max_count = 9,
			.fake_line_data = {
				{b00011000, 10 * 20},
				{b00011000, 10 * 20},
				{b00011000, 10 * 10},
				{b00011000, 3},
				{b11100000, 10 * 20},
				{b11100000, 10 * 10},
				{b11100000, 10 * 10},
				{b11100000, 10 * 10},
				{b00011000, 10 * 10}
			},
		}
	},
};

#endif /* SRC_APP_TRACK_DATA_H_ */
