/*
 * globals.h
 *
 *  Created on: 15/10/2017
 *      Author: Miguel
 */

#ifndef LIB_GLOBALS_H_
#define LIB_GLOBALS_H_

/*** CAR VERSION ****/
#ifndef CAR_YEAR
#define CAR_YEAR 2018
#endif
/********************/

/******** FEATURES SUPPORTED ********/
#define ENABLE_DEBUGGING           (1)
#define ENABLE_COMMUNICATIONS      (1)
#define ENABLE_BOOTLOADER          (0)
#define ENABLE_SPWM                (1)
#define ENABLE_MOTORS              (1)
#define ENABLE_MOTORS_SAFEMODE     (1)
#if CAR_YEAR == 2018
#define ENABLE_RPM_COUNTER         (0)
#else
#define ENABLE_RPM_COUNTER         (0)
#endif
#define ENABLE_SERVO               (1)
#define ENABLE_ACCELEROMETER       (0)
#if CAR_YEAR == 2018
#define ENABLE_SOUND               (1)
#else
#define ENABLE_SOUND               (0)
#endif
#define ENABLE_PID                 (1)
#define ENABLE_DYNAMIC_PID         (0)
#if CAR_YEAR == 2018
#define ENABLE_DYNAMIC_MOMENTUM    (1)
#else
#define ENABLE_DYNAMIC_MOMENTUM    (0)
#endif
#define ENABLE_STARTSWITCH         (1)
#define ENABLE_DIPSWITCH           (1)
#define ENABLE_MOTOR_CTRL_LEDS     (1)
#define ENABLE_STATUS_LOGGER       (1)
#define ENABLE_SHELL               (1)
#define ENABLE_DEBUG_LEDS          (1)
#define ENABLE_TEMPLATE_GENERATION (0)
#define ENABLE_REMOTE_CONTROL_MODE (1)
/************************************/

#include <platform.h>
#include <pin_mapping.h>

/******** RTOS APPLICATION DEFINITIONS **************************************************************************/
#define APP_CFG_POLLFREQ             OS_CFG_TICK_RATE_HZ /* Frequency of the polling                            */
#define RTOS_PROCESS_STACKSIZE       150
#define RTOS_PROCESS_DEFAULTPRIORITY 5
#if ENABLE_SOUND == 0
#if ENABLE_STATUS_LOGGER == 0
#define RTOS_PROCESS_MAXCOUNT 3
#else
#define RTOS_PROCESS_MAXCOUNT 4
#endif
#else
#define RTOS_PROCESS_MAXCOUNT 5
#endif
/** INFO: RTOS tasks allocated:
 * 0- main_app            @ src/app_main.c line 132
 * 1- suart_rx_cbk_dspchr @ lib/drivers/communications/protocols/suart/suart.c line 372
 * 2- packetman_task      @ lib/drivers/communications/protocols/packetman/packetman.c line 376
 * 3- piezo_player        @ lib/drivers/sound/piezo.c line 74
 * 4- status_logger_task  @ src/app_main.c line 431
 * 5- shell_task          @ src/app_main.c line 436
/****************************************************************************************************************/

/******** APPLICATION STATUS DEFINITIONS ************************************************************************/
#define STATUS_LED_FREQ 4000 /* PWM frequency of the status LEDs                                                */
#define SERVO_FREQ      48.8 /* PWM frequency of the servo motor (NOTE: multimeter reads 50.08 Hz)              */
#define MOTOR_FREQ      63   /* PWM frequency of the two motors                                                 */
#define ACCEL_FREQ      100  /* PWM frequency of the 2-axis accelerometer                                       */
#define SOUND_MAX_FREQ  4186 /* Maximum PWM frequency of the piezo buzzer                                       */
/****************************************************************************************************************/

/********* PID CONTROLLER DEFINITIONS ***************************/
#if CAR_YEAR == 2018
#define PID_KP       1.2    /* Normal operation P coefficient */
#define PID_KI       0.00   /* Normal operation I coefficient */
#define PID_KD       -90    /* Normal operation D coefficient */
#else
#define PID_KP       1.1    /* Normal operation P coefficient */
#define PID_KI       0.0012 /* Normal operation I coefficient */
#define PID_KD       -180   /* Normal operation D coefficient */
#endif

#define INT_WIND_PERIOD 10  /* The period (ms) at which the integral is reset */

#define MOMENTUM_EASE_ENABLE_THRESHOLD  9000 /* Counter threshold which enables the dynamic momentum ease feature  */
#define MOMENTUM_EASE_DISABLE_THRESHOLD 1000 /* Counter threshold which disables the dynamic momentum ease feature */

/****************************************************************/

/********* BRAKE DEFINITIONS ************************************/
#if CAR_YEAR == 2018
#define BRAKE_CORNER_SPEED_PERCENTAGE 10
#else
#define BRAKE_CORNER_SPEED_PERCENTAGE 40
#endif

#if CAR_YEAR == 2018
#define BRAKE_LANE_CHANGE_NOMOMENTUM   15
#define BRAKE_LANE_CHANGE_WITHMOMENTUM 15
#else
#define BRAKE_LANE_CHANGE_NOMOMENTUM   40
#define BRAKE_LANE_CHANGE_WITHMOMENTUM 40
#endif
/****************************************************************/

/********* ALGORITHM FSM DEFINITIONS ****************************/
enum MODE {
	MODE_NULL,                 /* (0)  An unknown state that should not be recognized by the algorithm                                                   */
	MODE_WAIT_FOR_STARTSWITCH, /* (1)  Wait for the start switch to be pressed before starting the race                                                  */
	MODE_FOLLOW_NORMAL_TRACE,  /* (2)  We are tracing a straight or slightly curved line within controlled conditions                                    */
	MODE_AVOID_RIGHT_BOUNDARY, /* (3)  The sensor detected a line at the right extreme sensors. We must check if it's a white tape, and if not, avoid it */
	MODE_AVOID_LEFT_BOUNDARY,  /* (4)  The sensor detected a line at the left extreme sensors.  We must check if it's a white tape, and if not, avoid it */
	MODE_FOUND_LEFT_TAPE,      /* (5)  We have encountered the white tape on the left side of the track                                                  */
	MODE_FOUND_RIGHT_TAPE,     /* (6)  We have encountered the white tape on the right side of the track                                                 */
	MODE_ALIGN_BOUNDARY,       /* (7)  We are currently aligning the car with the left/right side of the track                                           */
	MODE_TURNING_LANE,         /* (8)  We are currently turning the car through a lane change in basic or advanced mode                                  */
	MODE_TURNING_CORNER,       /* (9)  We are currently turning the car through a 90 degree corner in basic or advanced mode                             */
	MODE_TURNING_CORNER_BLIND, /* (10) We are currently turning the car through a lane change/90 degree blindly in smart mode                            */
	MODE_ACCIDENT,             /* (11) We have gone off track. Wait until the user puts the car back on the track and presses the button                 */
	MODE_RACE_COMPLETE,        /* (12) We have completed the race                                                                                        */
	MODE_REMOTE                /* (13) The car is being controlled/interacting with the user (not in race mode)                                          */
};
/****************************************************************/

/********* RACING / TRACK DEFINITIONS ******************************************************************/
#define TRACK_TYPE                2   /* Options: 0: FULL, 1: TEST1, 2: TEST2, 3: TEST3                */
#define LAP_MAX_COUNT             5   /* How many laps will we make                                    */
#define LAP_MAX_TURNS             7   /* How many 90 degree / lane change turns exist on the track     */
#define FLOOR_IS_ON               1   /* The floor reflects the sensor's IR light (1) or not (0)       */
#define PATTERN_MAP_SIZE          19  /* Total amount of sensor patterns we are going to try to match  */
#define TEMPLATE_MAX_SAMPLES      300 /* How many samples shall we use when generating a template line */
#define FAKE_LINEDATA_MAX_SAMPLES 1   /* How many fake line data samples in total shall we use         */
/** NOTE: it's possible to use less samples through the variable 'fake_line_data_max_count' ************/

enum TURN_DIRECTION {
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
	float     mapped_angle;
	float     desired_motor_left;
	float     desired_motor_right;
	float     p, i, d;
	enum MODE change_mode;
	int8_t    delta_momentum;
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
	uint8_t             corner_fetch_pattern;
	float               corner_brake_speed;
} turn_t;

enum INTEL {
	INTEL_BASIC,    /* Brakes down every white tape detection. Stays in the middle of the track.                                                                                 */
	INTEL_ADVANCED, /* Brakes down every white tape detection. Aligns itself in the right/left side of the track right before reaching apex.                                     */
	INTEL_SMART     /* Brakes down only on 90 degree corners. Ignores line sensor when detects a line, but still reads it to prevent any accident. Relies on distance travelled. */
};

typedef struct {
	/* Current global mode of the car's algorithm */

	enum INTEL intelligence_level;

	enum MODE mode;
	enum MODE last_mode;
	enum MODE next_mode;

	/* Tells the car how to react whenever it detects a certain pattern */
	pattern_map_t pattern_map[PATTERN_MAP_SIZE];

	/* Allows the car to learn and predict 90 degree corners */
	turn_t   incoming_turn[LAP_MAX_TURNS];
	turn_t * next_turn;

	/* How many 90 degree turns have we encountered so far */
	int turn_counter;

	int laps_completed;

	uint32_t line_misread_danger_counter;

	bool is_turning_lane;   /* Is the car currently going through a lane change? (regardless of the intelligence level)      */
	bool is_turning_corner; /* Is the car currently going through a 90 degree corner? (regardless of the intelligence level) */

	bool rcmode_persistant;

	bool race_started;

	uint32_t timeout_disable_control;

	int  momentum_counter;
	int  momentum_map_trigger_counter;
	bool momentum_map_triggered;

	bool servo_override;
} track_t;
/******************************************************************************************************/

#endif /* LIB_GLOBALS_H_ */
