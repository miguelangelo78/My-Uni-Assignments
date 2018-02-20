/*
 * servo.h
 *
 *  Created on: 18/10/2017
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_ACTUATORS_SERVO_SERVO_H_
#define LIB_DRIVERS_ACTUATORS_SERVO_SERVO_H_

#include <stdint.h>
#include <stdbool.h>
#include <libs/spwm/spwm.h>

#define SERVO_ANGLE_RANGE       (180)
#define SERVO_MIN_ANGLE         (-(SERVO_ANGLE_RANGE / 2))
#define SERVO_CENTER_ANGLE      (0)
#define SERVO_MAX_ANGLE         (SERVO_ANGLE_RANGE / 2)
#define SERVO_MIN_ANGLE_DUTY    (10.14f) /* Note: servo is installed in reverse ... */
#define SERVO_CENTER_ANGLE_DUTY (8.09f)
#define SERVO_MAX_ANGLE_DUTY    (6.04f)  /* Note: servo is installed in reverse ... */

enum SERVO_RETCODE {
	SERVO_OK,
	SERVO_ERR_ALREADY_INIT,
	SERVO_ERR_INVAL_ANGLE,
	SERVO_ERR_INVAL_HANDLE,
	SERVO_ERR_INVAL_ARGS,
	SERVO_ERR__COUNT /* How many different types of errors there are */
};

typedef struct {
	spwm_t * dev_handle;
	float    angle;
	float    angle_old;
	bool     is_locked;
	bool     is_sweeping;
} servo_t;

servo_t *          servo_init(void);
enum SERVO_RETCODE servo_reset(servo_t * handle);
enum SERVO_RETCODE servo_ctrl(servo_t * handle, float angle);
enum SERVO_RETCODE servo_accum_ctrl(servo_t * handle, float sum_angle);
enum SERVO_RETCODE servo_center(servo_t * handle);
enum SERVO_RETCODE servo_lock(servo_t * handle);
enum SERVO_RETCODE servo_unlock(servo_t * handle);
enum SERVO_RETCODE servo_sweep(servo_t * handle, float min_angle, float max_angle, uint32_t delay, float increment, bool from_cur_angle);

#endif /* LIB_DRIVERS_ACTUATORS_SERVO_SERVO_H_ */
