/*
 * mathutils.c
 *
 *  Created on: 16/11/2017
 *      Author: Miguel
 */

#include "mathutils.h"

#include "mathutils.h"

long map(long x, long in_min, long in_max, long out_min, long out_max) {
	if(x < in_min) x = in_min;
	if(x > in_max) x = in_max;
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
	if(x < in_min) x = in_min;
	if(x > in_max) x = in_max;
	return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}
