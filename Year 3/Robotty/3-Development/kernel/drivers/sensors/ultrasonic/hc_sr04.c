/*
 * hc_sr04.c
 *
 *  Created on: 03/03/2017
 *      Author: Miguel
 */

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <drivers/onchip/r_gpio_rx/r_gpio_rx_if.h>
#include <globals.h>

#if (1) /**** REGION: HC-SR04 DRIVER PRIVATE IMPLEMENTATION ****/
/************************************/
/****** PRIVATE IMPLEMENTATION ******/
/************************************/
#define ULTRASONIC_POLLING_PRESCALER 1 /* We'll sample the Ultrasonic sensor's pin at this prescaler value (which depends strongly on the frequency of the polling frequency of the ISR) */
#define ULTRASONIC_TIME_TO_DISTANCE_CM(time_elapsed) ((float)(time_elapsed) / 29.0f)/*58.2f*/ /* Convert microseconds to centimeter */
#define ULTRASONIC_MINIMUM_DISTANCE  2 /* The HC-SR04 cannot accurately detect objects closer than this distance (in centimeters) */

bool usonic_initialized = false;
uint32_t usonic_alloc_channel = 0;
volatile usonic_t usonic_channels[USONIC_MAX_CHANNEL_COUNT];
volatile uint8_t  usonic_poll_idx = 0;

bool usonic_is_frequency_respected(usonic_t * handle) {
	if(handle->frequency_prescaler++ < ULTRASONIC_POLLING_PRESCALER) return false;
	handle->frequency_prescaler = 0;
	return true;
}

void usonic_poll_single_channel(usonic_t * chan) {
	if(!usonic_is_frequency_respected(chan)) return;

	switch(chan->delta_poll) {
	case 0: /* Set trigger high */
		chan->delta_poll++;
		R_GPIO_PinWrite(chan->trigpin, GPIO_LEVEL_HIGH);
		break;
	case 1: /* Set trigger off */
		chan->delta_poll++;
		R_GPIO_PinWrite(chan->trigpin, GPIO_LEVEL_LOW);
		break;
	case 2: /* Wait for Echo to become high */
		if(R_GPIO_PinRead(chan->echopin)) {
			chan->delta_poll++;
		} else {
			if(++chan->pulse_timeout >= USONIC_MAX_PULSE_TIMEOUT) {
				/* Pulse has timed out and we did not receive any sound reflection at all */
				chan->delta_poll = 5;
			}
		}
		break;
	case 3: /* Start counting the time echo stays high */
		if(R_GPIO_PinRead(chan->echopin)) {
			if(++chan->echo_highcounter >= USONIC_MAX_PULSE_HIGHLEVEL_TIMEOUT) {
				/* The HC-SR04 seems to be stuck on setting the echo pin high... Maybe we should try re-triggering the sensor */
				chan->delta_poll = 5;
			}
		} else {
			chan->delta_poll++;
		}
		break;
	case 4: {
			/* Convert + Push the counted time into the circular buffer */
			float time_to_distance = ULTRASONIC_TIME_TO_DISTANCE_CM(chan->echo_highcounter * 10);
			if(time_to_distance >= ULTRASONIC_MINIMUM_DISTANCE) /* We should really only push when the values are realistic. */
				circular_push_float(chan->circ_buff, time_to_distance);
			chan->delta_poll++;
		}
		break;
	case 5: /* Reset counters */
		chan->echo_highcounter = 0;
		chan->pulse_timeout    = 0;
		chan->delta_poll       = 0;
		if(++usonic_poll_idx >= USONIC_MAX_CHANNEL_COUNT)
			usonic_poll_idx = 0;
		break;
	default: break;
	}
}

void usonic_calculator(void * args) {
	/* Make calculations for this particular channel */
	usonic_t * channel = (usonic_t*)args;

	while(1) {
		/* ***********************************************
		 * Make calculations on the sampled values here: *
		 * ***********************************************/

		/* Calculating average distance: */
		float temp_avg_distance = 0;
		for(int i = 0; i < channel->circ_buff->bufflen; i++)
			temp_avg_distance += channel->circ_buff->buff_float[i];
		temp_avg_distance /= channel->circ_buff->bufflen;

		if(temp_avg_distance >= ULTRASONIC_MINIMUM_DISTANCE)
			channel->avg_distance = usonic_distance_cm_adjust(temp_avg_distance);

		rtos_preempt();
	}
}

void usonic_init_all_channels(void) {
	for(uint8_t i = 0; i < USONIC_MAX_CHANNEL_COUNT; i++) {
		/* Initialize Ultrasonic sensor's struct variables: */
		usonic_channels[i].delta_poll          = 0;
		usonic_channels[i].frequency_prescaler = 0;
		usonic_channels[i].circ_buff           = 0;
		usonic_channels[i].echo_highcounter    = 0;
		usonic_channels[i].pulse_timeout       = 0;
		usonic_channels[i].echopin             = (gpio_port_pin_t)0;
		usonic_channels[i].trigpin             = (gpio_port_pin_t)0;
		usonic_channels[i].is_init             = false;
	}
}
#endif

#if (1) /**** REGION: HC-SR04 DRIVER PUBLIC IMPLEMENTATION ****/
/***********************************/
/****** PUBLIC IMPLEMENTATION ******/
/***********************************/
void usonic_poll(void) {
	usonic_t * chan = (usonic_t*)&usonic_channels[usonic_poll_idx];
	if(chan->is_init)
		usonic_poll_single_channel(chan);
}

usonic_t * usonic_init(gpio_port_pin_t echopin, gpio_port_pin_t trigpin, uint32_t bufflen) {
	if(usonic_alloc_channel >= USONIC_MAX_CHANNEL_COUNT) return (usonic_t*)USONIC_ERR_BADINIT;

	usonic_t * channel_ptr = (usonic_t*)USONIC_ERR_BADINIT;

	/* Only initialize the new channel if the buffer length is larger than 0 and the pins are non-zero: */
	if(bufflen > 0 && echopin && trigpin) {
		if(!usonic_initialized) {
			usonic_init_all_channels();
			usonic_initialized = true;
		}

		/* Initialize channel's variables: */
		usonic_channels[usonic_alloc_channel].circ_buff = circular_float_create(bufflen);
		usonic_channels[usonic_alloc_channel].echopin   = echopin;
		usonic_channels[usonic_alloc_channel].trigpin   = trigpin;

		/* Initialize pins: */
		R_GPIO_PinDirectionSet(echopin, GPIO_DIRECTION_INPUT);
		R_GPIO_PinDirectionSet(trigpin, GPIO_DIRECTION_OUTPUT);

		/* Fetch this channel's address: */
		channel_ptr = (usonic_t*)&usonic_channels[usonic_alloc_channel];

		/* Create RTOS task for generating pre-calculated values: */
		spawn_task_args("usonic_calculator", usonic_calculator, channel_ptr);

		/* Make next channel available for allocation: */
		usonic_channels[usonic_alloc_channel++].is_init = true;
	}

	return channel_ptr;
}

float usonic_get_avg_distance(usonic_t * handle) {
	if(!handle) return USONIC_ERR_BADHANDLE;
	return handle->avg_distance;
}

float usonic_get_avg_distance_meters(usonic_t * handle) {
	return usonic_get_avg_distance(handle) / 10;
}

#endif
