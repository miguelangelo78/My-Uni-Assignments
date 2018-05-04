/*
 * hc_sr04.h
 *
 *  Created on: 03/03/2017
 *      Author: Miguel
 */

#ifndef DRIVERS_SENSORS_ULTRASONIC_HC_SR04_H_
#define DRIVERS_SENSORS_ULTRASONIC_HC_SR04_H_

#include <stdbool.h>
#include <stdint.h>
#include <libs/data_structures/circular_buffer.h>

#define USONIC_MAX_CHANNEL_COUNT           2
#define USONIC_MAX_PULSE_TIMEOUT           1000
#define USONIC_MAX_PULSE_HIGHLEVEL_TIMEOUT 1000

enum USONIC_ERRCODE {
	USONIC_ERR_BADHANDLE,
	USONIC_OK,
	USONIC_ERR_BADINIT,
	USONIC_ERR_NOSAMPLES,
	USONIC_ERR__COUNT /* How many different types of errors there are */
};

typedef struct {
	bool is_init;
	gpio_port_pin_t trigpin;
	gpio_port_pin_t echopin;
	int32_t         frequency_prescaler;
	uint8_t         delta_poll;
	uint32_t        echo_highcounter;
	uint32_t        pulse_timeout;
	circular_t    * circ_buff;
	/* Pre-Calculated values: */
	float           avg_distance;
} usonic_t;

void usonic_poll(void);
usonic_t * usonic_init(gpio_port_pin_t echopin, gpio_port_pin_t trigpin, uint32_t bufflen);
float usonic_get_avg_distance(usonic_t * handle); /* Return distance in centimeters */
float usonic_get_avg_distance_meters(usonic_t * handle); /* Return distance in meters */
#define usonic_distance_cm_adjust(cm) ((cm) - 2) /* This macro adjusts the distance measured to get the REAL distance. For some reason, we are getting an offset distance of 2 cm */

#endif /* DRIVERS_SENSORS_ULTRASONIC_HC_SR04_H_ */
