/*
 * status_led.c
 *
 *  Created on: 14/03/2017
 *      Author: Miguel
 */

#include <globals.h>
#include <libs/utils.h>
#include <drivers/onchip/r_gpio_rx/r_gpio_rx_if.h>

#if (1) /**** REGION: STATUS LED DRIVER PRIVATE IMPLEMENTATION ****/
/************************************/
/****** PRIVATE IMPLEMENTATION ******/
/************************************/
bool     sled_initialized   = false;
uint32_t sled_alloc_channel = 0;
volatile sled_t sled_channels[SLED_MAX_CHANNEL_COUNT];

void sled_update_single_led(spwm_t * pwm_channel, uint8_t brightness) {
	/* Map the brightness value from 0->255 to 0->100 (0% -> 100% duty cycle): */
	brightness = (uint8_t)map(brightness, 0, 255, 0, 100);
	spwm_set_duty(pwm_channel, 100 - brightness);
}

void sled_poller(void * args) {
	sled_t * channel = (sled_t*)args;

	/* Make LED animations and set the right colors based on the variables 'notification' and 'animation' from the struct: */

	rtos_preempt();

	while(1) {
		uint8_t up_delta_anim       = 0;
		uint8_t down_delta_anim     = 0;
		uint8_t up_delta_delay      = 1;
		uint8_t down_delta_delay    = 1;
		bool    enable_red          = false;
		bool    enable_green        = false;
		bool    enable_blue         = false;
		static uint8_t idle_counter = 0;
		bool    enable_idle         = false;

		switch(channel->notification) {
		case STATUS_NOTE_OK:
			enable_green = true;
			break;
		case STATUS_NOTE_INFO:
			enable_blue = true;
			break;
		case STATUS_NOTE_WARNING:
			enable_red = enable_green = true;
			break;
		case STATUS_NOTE_PANIC:
			enable_red = true;
			break;
		case STATUS_NOTE_IDLE:
			enable_idle  = true;
			if(!idle_counter)
				idle_counter = 2;
			break;
		case STATUS_NOTE_DEMO:
			/* TODO: Unimplemented */
			break;
		case STATUS_NOTE_DEBUG:
			/* TODO: Unimplemented */
			break;
		default: break;
		}

		if(!enable_idle)
			idle_counter = 0;

		switch(channel->animation) {
			case STATUS_ANIM_NONE: up_delta_anim = down_delta_anim = 0; break;
			case STATUS_ANIM_SLOWFADE:
				up_delta_anim   = 5;
				down_delta_anim = 5;
				break;
			case STATUS_ANIM_FASTFADE:
				up_delta_anim   = 20;
				down_delta_anim = 20;
				break;
			case STATUS_ANIM_SLOWBLINK:
				up_delta_anim    = 255;
				down_delta_anim  = 255;
				up_delta_delay   = 10;
				down_delta_delay = 20;
				break;
			case STATUS_ANIM_FASTBLINK:
				up_delta_anim    = 255;
				down_delta_anim  = 255;
				up_delta_delay   = 2;
				down_delta_delay = 4;
				break;
			default: break;
		}

		/* Reset all the SPWM channels first: */
		sled_update_single_led(channel->red_led,   0);
		sled_update_single_led(channel->green_led, 0);
		sled_update_single_led(channel->blue_led,  0);

		/* Now update the LEDs using the determined parameters: */
		for(int i = 0; i < 255; i += up_delta_anim) {
			if(enable_red   || (idle_counter & 1)) sled_update_single_led(channel->red_led,   i);
			if(enable_green || (idle_counter & 2)) sled_update_single_led(channel->green_led, i);
			if(enable_blue  || (idle_counter & 4)) sled_update_single_led(channel->blue_led,  i);
			for(uint8_t j = 0; j < up_delta_delay; j++) {
				rtos_delay(10);
				if(channel->anim_update_flag)
					break;
			}
			if(channel->anim_update_flag)
				break;
		}

		for(int i = 255; i >= 0; i -= down_delta_anim) {
			if(channel->anim_update_flag) {
				channel->anim_update_flag = false;
				break;
			}
			if(enable_red   || (idle_counter & 1)) sled_update_single_led(channel->red_led,   i);
			if(enable_green || (idle_counter & 2)) sled_update_single_led(channel->green_led, i);
			if(enable_blue  || (idle_counter & 4)) sled_update_single_led(channel->blue_led,  i);
			for(uint8_t j = 0; j < down_delta_delay; j++) {
				rtos_delay(10);
				if(channel->anim_update_flag)
					break;
			}
		}

		if(enable_idle) {
			if(++idle_counter > 7)
				idle_counter = 0;
		}
	}
}

void sled_init_all_channels(void) {
	for(uint8_t i = 0; i < SLED_MAX_CHANNEL_COUNT; i++) {
		sled_channels[i].red_led          = 0;
		sled_channels[i].green_led        = 0;
		sled_channels[i].blue_led         = 0;
		sled_channels[i].notification     = STATUS_NOTE_INVAL;
		sled_channels[i].animation        = STATUS_ANIM_INVAL;
		sled_channels[i].anim_update_flag = false;
		sled_channels[i].is_init          = false;
	}
}

#endif

#if (1) /**** REGION: STATUS LED DRIVER PUBLIC IMPLEMENTATION ****/
/***********************************/
/****** PUBLIC IMPLEMENTATION ******/
/***********************************/
sled_t * status_led_init(gpio_port_pin_t redpin, gpio_port_pin_t greenpin, gpio_port_pin_t bluepin) {
	if(sled_alloc_channel >= SLED_MAX_CHANNEL_COUNT) return (sled_t*)SLED_ERR_BADINIT;

	sled_t * channel_ptr = (sled_t*)SLED_ERR_BADINIT;

	/* Only initialize the new channel if all the output pins are non-zero */
	if(redpin && greenpin && bluepin) {
		if(!sled_initialized) {
			sled_init_all_channels();
			sled_initialized = true;
		}

		/* Initialize channel's variables: */
		sled_channels[sled_alloc_channel].red_led      = spwm_create(SLED_MAX_FREQUENCY, 100, SLED_DEFAULT_SPWM_MODE, 0, redpin);
		sled_channels[sled_alloc_channel].green_led    = spwm_create(SLED_MAX_FREQUENCY, 100, SLED_DEFAULT_SPWM_MODE, 0, greenpin);
		sled_channels[sled_alloc_channel].blue_led     = spwm_create(SLED_MAX_FREQUENCY, 100, SLED_DEFAULT_SPWM_MODE, 0, bluepin);
		sled_channels[sled_alloc_channel].notification = SLED_DEFAULT_NOTIFICATION;
		sled_channels[sled_alloc_channel].animation    = SLED_DEFAULT_ANIMATION;

		/* Fetch this channel's address: */
		channel_ptr = (sled_t*)&sled_channels[sled_alloc_channel];

		/* Create RTOS task for polling the SLED animation algorithm: */
		spawn_task_args("sled_poller", sled_poller, channel_ptr);

		/* Make next channel available for allocation: */
		sled_channels[sled_alloc_channel++].is_init = true;
	}

	return channel_ptr;
}

enum SLED_ERR_CODE status_led_update(sled_t * handle, enum STATUS_NOTIFICATION_TYPE notification, enum STATUS_ANIMATION_TYPE animation) {
	if(!handle)          return SLED_ERR_BADHANDLE;
	if(!handle->is_init) return SLED_ERR_BADINIT;
	if(handle->notification == STATUS_NOTE_INVAL || handle->notification >= STATUS_NOTE__COUNT) return SLED_ERR_BADARGS;
	if(handle->animation    == STATUS_ANIM_INVAL || handle->animation    >= STATUS_ANIM__COUNT) return SLED_ERR_BADARGS;

	handle->notification     = notification;
	handle->animation        = animation;
	handle->anim_update_flag = true;

	return SLED_OK;
}
#endif
