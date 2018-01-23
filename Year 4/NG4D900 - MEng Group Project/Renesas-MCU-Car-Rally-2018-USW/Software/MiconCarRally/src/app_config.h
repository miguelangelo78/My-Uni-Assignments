/*
 * globals.h
 *
 *  Created on: 15/10/2017
 *      Author: Miguel
 */

#ifndef LIB_GLOBALS_H_
#define LIB_GLOBALS_H_

#include <platform.h>

/******** FEATURES SUPPORTED ********/
#define ENABLE_DEBUGGING           (1)
#define ENABLE_COMMUNICATIONS      (1)
#define ENABLE_MOTORS              (1)
#define ENABLE_MOTORS_SAFEMODE     (1)
#define ENABLE_SERVO               (1)
#define ENABLE_LTRACKER            (1)
#define ENABLE_PID                 (1)
#define ENABLE_STARTSWITCH         (1)
#define ENABLE_MOTOR_CTRL_LEDS     (1)
#define ENABLE_STATUS_LOGGER       (1)
#define ENABLE_SHELL               (1)
#define ENABLE_DEBUG_LEDS          (1)
#define ENABLE_TEMPLATE_GENERATION (0)
/************************************/

/******** RTOS APPLICATION DEFINITIONS **************************************************************************/
#define APP_CFG_POLLFREQ       OS_CFG_TICK_RATE_HZ /* Frequency of the polling                                  */
/****************************************************************************************************************/

/********* PID CONTROLLER DEFINITIONS ***************************/
#define HANDLE_KP       2.1   /* Normal operation P coefficient */
#define HANDLE_KI       0.066 /* Normal operation I coefficient */
#define HANDLE_KD       37    /* Normal operation D coefficient */

#define CRANK_HANDLE_KP 1     /* Crank operation P coefficient  */
#define CRANK_HANDLE_KI 0     /* Crank operation I coefficient  */
#define CRANK_HANDLE_KD 10    /* Crank operation D coefficient  */

#define BRAKE_KP        2     /* Braking P coefficient          */
#define BRAKE_KI        0     /* Braking I coefficient          */
#define BRAKE_KD        0     /* Braking D coefficient          */
/****************************************************************/

/********* ALGORITHM FSM DEFINITIONS ****************************/
enum MODE {
	MODE_NULL,                   /* (0) An unknown state that should not be recognized by the algorithm                                                   */
	MODE_WAIT_FOR_STARTSWITCH,   /* (1) Wait for the start switch to be pressed before starting the race                                                  */
	MODE_FOLLOW_NORMAL_TRACE,    /* (2) We are tracing a straight or slightly curved line within controlled conditions                                    */
	MODE_AVOID_RIGHT_BOUNDARY,   /* (3) The sensor detected a line at the right extreme sensors. We must check if it's a white tape, and if not, avoid it */
	MODE_AVOID_LEFT_BOUNDARY,    /* (4) The sensor detected a line at the left extreme sensors.  We must check if it's a white tape, and if not, avoid it */
	MODE_FOUND_LEFT_TAPE,        /* (5) We have encountered the white tape on the left side of the track                                                  */
	MODE_FOUND_RIGHT_TAPE,       /* (6) We have encountered the white tape on the right side of the track                                                 */
	MODE_TURNING_CORNER,         /* (7) We are currently turning the car through a 90 degree corner / lane change                                         */
	MODE_RACE_COMPLETE           /* (8) We have completed the race                                                                                        */
};
/****************************************************************/

/******** PHYSICAL PROPERTIES DEFINITIONS ****************/
#define WHEEL_LENGTH 0.132 /* Car wheel base length in m */
#define WHEEL_WIDTH  0.160 /* Car wheel base width  in m */
/*********************************************************/

/********* RACING / TRACK DEFINITIONS ******************************************************************/
#define LAP_MAX_COUNT             3   /* How many laps will we make                                    */
#define LAP_MAX_TURNS             1   /* How many 90 degree / lane change turns exist on the track     */
#define PATTERN_MAP_SIZE          15  /* Total amount of sensor patterns we are going to try to match  */
#define TEMPLATE_MAX_SAMPLES      300 /* How many samples shall we use when generating a template line */
#define FAKE_LINEDATA_MAX_SAMPLES 30  /* How many fake line data samples in total shall we use         */
/** NOTE: it's possible to use less samples through the variable 'fake_line_data_max_count' ************/

enum TURN_DIRECTION {
	TURN_UNKNOWN,
	TURN_LEFT,
	TURN_RIGHT
};

/*
 * The struct below tells the car how to control the
 * servo and motors for every
 * pattern detected
 */
typedef struct {
	uint8_t   pattern;
	float     desired_angle;
	float     desired_motor_left;
	float     desired_motor_right;
	enum MODE change_mode;
} pattern_map_t;

typedef struct {
	uint8_t  error_pattern;
	uint32_t correction_period;
	float    left_motor_brake_speed;
	float    right_motor_brake_speed;
} fake_line_data_t;

/*
 * We'll use the struct below to let the car know
 * when to 'pre-turn' before it meets the actual
 * 90 degree turn / lane change
 */
typedef struct {
	enum TURN_DIRECTION direction;
	bool                is_lane_change;
	uint16_t            distance;
	fake_line_data_t    fake_line_data[FAKE_LINEDATA_MAX_SAMPLES];
	uint16_t            fake_line_data_max_count;
	uint16_t            fake_line_data_index;
} turn_t;

typedef struct {
	/* Current global mode of the car's algorithm */
	enum MODE mode;
	enum MODE last_mode;
	enum MODE next_mode;

	/* Tells the car how to react whenever it detects a certain pattern */
	pattern_map_t pattern_map[PATTERN_MAP_SIZE];

	/* Allows the car to learn and predict 90 degree corners */
	turn_t   incoming_turn[LAP_MAX_TURNS + 1];
	turn_t * next_turn;

	/* How many 90 degree turns have we encountered so far */
	int turn_counter;

	int laps_completed;
} track_t;
/******************************************************************************************************/

#endif /* LIB_GLOBALS_H_ */
