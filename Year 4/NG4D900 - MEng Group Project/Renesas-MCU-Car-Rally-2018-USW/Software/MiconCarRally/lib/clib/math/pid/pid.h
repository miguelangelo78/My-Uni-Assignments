/*
 * pid.h
 *
 *  Created on: 08/11/2017
 *      Author: Miguel
 */

#ifndef LIB_MATH_PID_PID_H_
#define LIB_MATH_PID_PID_H_

#include <stdbool.h>
#include <stdlib.h>

typedef struct {
	bool init;
	float feedback;
	float kp, ki, kd;
	float proportional;
	float integral;
	float derivative;
	float error;
	float output;
} pid_t;

pid_t * pid_new_controller(float kp, float ki, float kd);
float   pid_control(pid_t * pid_handle);
bool    pid_update_feedback(pid_t * pid_handle, float new_feedback);
bool    pid_change_kp(pid_t * pid_handle, float new_kp);
bool    pid_change_ki(pid_t * pid_handle, float new_ki);
bool    pid_change_kd(pid_t * pid_handle, float new_kd);
bool    pid_change_constants(pid_t * pid_handle, float new_kp, float new_ki, float new_kd);
bool    pid_reset(pid_t * pid_handle);

#endif /* LIB_MATH_PID_PID_H_ */
