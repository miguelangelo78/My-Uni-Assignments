/*
 * app_car_control.h
 *
 *  Created on: 20/02/2018
 *      Author: Miguel
 */

#ifndef SRC_APP_CAR_CONTROL_H_
#define SRC_APP_CAR_CONTROL_H_

#include <stdbool.h>
#include <stddef.h>
#include <utils.h>

#include <rtos_inc.h>

#include <app_config.h>
#include <app_template_generator.h>

#include <math/pid/pid.h>
#include <actuators/servo/servo.h>
#include <sensors/ltracker/ltracker.h>
#include <actuators/motor_driver/motor_driver.h>
#include <sensors/accel/accel.h>
#include <sound/piezo.h>
#include <sound/tunes.h>
#include <onchip/led.h>
#include <onchip/switch.h>

/* Read white tape on both sides of the track */
#define check_crossline(sensor_data) ((sensor_data & MASK3_3) == b11100111)

/* Read white tape only on the right side */
#define check_rightline(sensor_data) (sensor_data == b00011111 || sensor_data == b00111111 || sensor_data == b01111111 || sensor_data == b00001111)

/* Read white tape only on the left side */
#define check_leftline(sensor_data)  (sensor_data == b11111000 || sensor_data == b11111100 || sensor_data == b11111110 || sensor_data == b11110000)

/* Check if we have recovered by avoiding the left edge */
#define check_left_boundary_recovery(sensor_data) (sensor_data == b01100000 || sensor_data == b11100000)

/* Check if we have recovered by avoiding the right edge */
#define check_right_boundary_recovery(sensor_data) (sensor_data == b00000110 || sensor_data == b00000111)

#define is_in_template_generation_mode() (uint8_t)((dipswitch_read() & 0x8) >> 3)

#define change_to_new_mode(new_mode, _next_mode) do { track.last_mode = track.mode; track.mode = new_mode; track.next_mode = _next_mode; } while(0)

#define change_to_next_mode(_next_mode) do { track.last_mode = track.mode; track.mode = track.next_mode; track.next_mode = _next_mode; } while(0)

extern track_t   track;
extern motor_t * module_left_wheel;
extern motor_t * module_right_wheel;
extern servo_t * module_servo;
extern accel_t * module_accel;
extern pid_t   * pid_controller;
extern float     pid_output;
extern piezo_t * module_piezo;

void    car_kickstart(void);
void    car_algorithm_poll(uint8_t line_sensor);
uint8_t car_update_control(void);
float   map_sensor_to_angle(uint8_t sensor_data);
void    log_unrecognized_pattern(uint8_t pattern);

#endif /* SRC_APP_CAR_CONTROL_H_ */
