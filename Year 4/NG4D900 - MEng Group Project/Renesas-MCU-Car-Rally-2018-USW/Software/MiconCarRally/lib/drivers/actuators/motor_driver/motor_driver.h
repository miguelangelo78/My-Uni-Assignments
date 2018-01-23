/*
 * motor_driver.h
 *
 *  Created on: 18/10/2017
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_ACTUATORS_MOTOR_DRIVER_H_
#define LIB_DRIVERS_ACTUATORS_MOTOR_DRIVER_H_

#include <stdint.h>
#include <stdbool.h>

#define MOTOR_MAX_PWM_SPEED      (24575)
#define MOTOR_MAX_RPM_SPEED      (2000)
#define MOTOR_SAFE_MODE_LEVEL    (3.26f) /* Expressed in % from 0 to 100 */
#define MOTOR_DEADBAND           (0)
#define MOTOR_RPM_MEASURE_PERIOD (15) /* The period in which we measure the motor's RPM (in ms) */

enum MOTOR_ERRCODE {
	MOTOR_OK,
	MOTOR_ERR_ALREADY_INIT,
	MOTOR_ERR_INVAL_CHANNEL,
	MOTOR_ERR_INVAL_DIRECTION,
	MOTOR_ERR_DIFFERENTIAL,
	MOTOR_ERR_SET_SPEED,
	MOTOR_ERR__COUNT /* How many different types of errors there are */
};

enum MOTOR_CHANNEL {
	MOTOR_CHANNEL_NULL,
	MOTOR_CHANNEL_LEFT,
	MOTOR_CHANNEL_RIGHT,
	MOTOR_CHANNEL__COUNT
};

typedef struct {
	float    speed; /* Expressed in % from 0 to 100 */
	float    speed_old;
	float    speed_safe_old;
	float    acceleration;
	float    rpm_measured;
	uint32_t rpm_counter;
	uint32_t rpm_counter_old;
	bool     is_braking;
	bool     is_safemode;
	enum MOTOR_CHANNEL side;
} motor_t;

motor_t * motor_init(enum MOTOR_CHANNEL channel);
motor_t * motor_init_safe(enum MOTOR_CHANNEL channel, bool enable_safemode);
enum MOTOR_ERRCODE motor_reset(motor_t * handle);
enum MOTOR_ERRCODE motor_stop(motor_t * handle);
enum MOTOR_ERRCODE motor_resume(motor_t * handle);
enum MOTOR_ERRCODE motor_ctrl(motor_t * handle, float speed_percentage);
enum MOTOR_ERRCODE motor_ctrl_with_differential(motor_t * handle, float speed_percentage, float pid_output);
enum MOTOR_ERRCODE motor_refresh(motor_t * handle);
enum MOTOR_ERRCODE motor_refresh_with_differential(motor_t * handle, float pid_output);
enum MOTOR_ERRCODE motor_set_safe_mode(motor_t * handle, bool enable, bool update_speed);
enum MOTOR_ERRCODE motor_set_speed(motor_t * handle, float speed_percentage);
enum MOTOR_ERRCODE motor_brake(motor_t * handle, bool is_breaking);
enum MOTOR_ERRCODE motor_rpm_sensor_poll(motor_t * handle);

#define rpmcounter_left_read()  (PORT1.PORT.BIT.B1 & 0x1)
#define rpmcounter_right_read() (PORT9.PORT.BIT.B6 & 0x1)

#endif /* LIB_DRIVERS_ACTUATORS_MOTOR_DRIVER_H_ */
