/*
 * ltracker.h
 *
 *  Created on: 01/11/2017
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_SENSORS_LTRACKER_LTRACKER_H_
#define LIB_DRIVERS_SENSORS_LTRACKER_LTRACKER_H_

#include <stddef.h>
#include <stdint.h>

/* Returns the line tracker's sensor values */
uint8_t ltracker_read(uint8_t mask, bool * motors_updated);

#endif /* LIB_DRIVERS_SENSORS_LTRACKER_LTRACKER_H_ */
