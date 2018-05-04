/*
 * mathutils.h
 *
 *  Created on: 16/11/2017
 *      Author: Miguel
 */

#ifndef LIB_MATH_MATHUTILS_H_
#define LIB_MATH_MATHUTILS_H_

#include <math.h>

#define PI 3.14159265

#define RAD(degree) ((degree) * (PI / 180))

long  map(long x, long in_min, long in_max, long out_min, long out_max);
float mapfloat(float x, float in_min, float in_max, float out_min, float out_max);

#endif /* LIB_MATH_MATHUTILS_H_ */
