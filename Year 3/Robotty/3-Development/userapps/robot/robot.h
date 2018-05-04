/*
 * robot.h
 *
 *  Created on: 16/03/2017
 *      Author: Miguel
 */

#ifndef ROBOT_ROBOT_H_
#define ROBOT_ROBOT_H_

/***********************************/
/** TOP LEVEL ROBOT DECLARATIONS: **/
/***********************************/
#define ROBOT_DEFAULT_MODE ROBOT_MODE_RCCAR
#define ROBOT_DEBUG_ENABLE (0) /* Enable (1) or Disable (0) Robot debugging messages out through the Bluetooth Module */
#define ROBOT_UPDATE_DELTA  1  /* How frequent the robot FSM logic is updated (in milliseconds) */
#define ROBOT_INVALID_LINE_FOUND_TIMEOUT 1000 /* How many delta cycles shall we wait until we give up on trying to find a good line again */
#define ROBOT_LOST_LINE_COOLDOWN         1000 /* How many delta cycles until we consider the car to be off track */

enum ROBOT_MODES {
	ROBOT_MODE_IDLE,
	ROBOT_MODE_AUTONOMOUS,   /* The car scans the environment avoiding obstacles using the two ultrasonic sensors */
	ROBOT_MODE_LINEFOLLOWER, /* The car follows a black line on a white background */
	ROBOT_MODE_RCCAR         /* Remote Control Car Mode */
};

enum BT_CODES {
	BT_INVAL =  0,
	NONE     = '0',
	UP       = '1',
	DOWN     = '2',
	LEFT     = '3',
	RIGHT    = '4',
	TRIANGLE = '7',
	CIRCLE   = 'A',
	CROSS    = '9',
	SQUARE   = '8',
	START    = '6',
	SELECT   = '5'
};

enum IR_CODES {
	IR_INVAL = 0,
	OK       = 5376,
	FORW     = 4352,
	BACK     = 6400,
	IR_LEFT  = 5120,
	IR_RIGHT = 5632
};

extern void start_robot(void * args);
extern void robot_on_ir(uint16_t code);
extern void robot_on_bt(uint8_t * buff, uint32_t bufflen);
/***********************************/

/*******************************/
/** ROBOT RC CAR DECLARATIONS **/
/*******************************/
extern void robot_rcc_on_bt(enum BT_CODES key);
extern void robot_rcc_on_ir(enum IR_CODES key);
/*******************************/

/***************************************/
/** ROBOT AUTONOMOUS CAR DECLARATIONS **/
/***************************************/
#define ROBOT_AUTON_MINIMUM1_WALL_DISTANCE_CM 20 /* How close we can get to the wall before considering a hazard (used in the bottom sensor) */
#define ROBOT_AUTON_MINIMUM2_WALL_DISTANCE_CM 12 /* How close we can get to the wall before considering a hazard (used in the top sensor)    */
extern void robot_auton_restart(void);
extern void robot_auton_update(void);
extern void robot_auton_enable_bottom_usonic(void);
extern void robot_auton_disable_bottom_usonic(void);
/***************************************/

/******************************************/
/** ROBOT LINE FOLLOWER CAR DECLARATIONS **/
/******************************************/
extern void robot_lfollower_restart(void);
extern bool robot_lfollower_update(void);
extern bool robot_lfollower_check_line(void);
extern bool robot_lfollower_is_line_lost_cooldown(void);
/******************************************/

#endif /* ROBOT_ROBOT_H_ */
