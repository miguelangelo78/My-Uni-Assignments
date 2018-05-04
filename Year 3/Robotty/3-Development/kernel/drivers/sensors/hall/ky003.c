/*
 * ky003.c
 *
 *  Created on: 08/03/2017
 *      Author: Miguel
 */

#include <drivers/onchip/r_gpio_rx/r_gpio_rx_if.h>
#include <globals.h>

#if (1) /**** REGION: KY003 DRIVER PRIVATE IMPLEMENTATION ****/
/************************************/
/****** PRIVATE IMPLEMENTATION ******/
/************************************/
bool     dhall_initialized   = false;
uint32_t dhall_alloc_channel = 0;
volatile dhall_t dhall_channels[DHALL_MAX_CHANNEL_COUNT];

void dhall_poller(void * args) {
	dhall_t * channel = (dhall_t*)args;

	while(1) {
		if(dhall_is_present(channel) == DHALL_DETECTED) {
			/* A magnetic field was detected. Call the callback at least once: */
			channel->cback(channel);

			while(dhall_is_present(channel) == DHALL_DETECTED) {
				/* A magnetic field is still being detected. */
				if(channel->mode == DHALL_MODE_PERSISTENT)
					/* Keep calling the callback in Persistent mode: */
					channel->cback(channel);
				rtos_preempt();
			}
			/* We're not detecting the magnetic field anymore */
		}
		rtos_preempt();
	}
}

void dhall_init_all_channels(void) {
	for(uint8_t i = 0; i < DHALL_MAX_CHANNEL_COUNT; i++) {
		dhall_channels[i].cback     = 0;
		dhall_channels[i].signalpin = (gpio_port_pin_t)0;
		dhall_channels[i].mode      = DHALL_MODE_NULL;
		dhall_channels[i].is_init   = false;
	}
}
#endif

#if (1) /**** REGION: KY003 DRIVER PUBLIC IMPLEMENTATION ****/
/***********************************/
/****** PUBLIC IMPLEMENTATION ******/
/***********************************/
dhall_t * dhall_init(gpio_port_pin_t signalpin, dhall_cback_t cback, enum DHALL_MODE mode) {
	if(dhall_alloc_channel >= DHALL_MAX_CHANNEL_COUNT) return (dhall_t*)DHALL_ERR_BADINIT;

	dhall_t * channel_ptr = (dhall_t*)DHALL_ERR_BADINIT;

	/* Only initialize the new channel if the input pin is non-zero, the callback is also non-zero and the mode is valid: */
	if(signalpin && cback && mode > DHALL_MODE_NULL && mode < DHALL_MODE__COUNT) {
		if(!dhall_initialized) {
			dhall_init_all_channels();
			dhall_initialized = true;
		}

		/* Initialize channel's variables: */
		dhall_channels[dhall_alloc_channel].signalpin = signalpin;
		dhall_channels[dhall_alloc_channel].cback     = cback;
		dhall_channels[dhall_alloc_channel].mode      = mode;

		/* Initialize pin: */
		R_GPIO_PinDirectionSet(signalpin, GPIO_DIRECTION_INPUT);

		/* Fetch this channel's address: */
		channel_ptr = (dhall_t*)&dhall_channels[dhall_alloc_channel];

		/* Create RTOS task for polling the signal pin: */
		spawn_task_args("dhall_poller", dhall_poller, channel_ptr);

		/* Make next channel available for allocation: */
		dhall_channels[dhall_alloc_channel++].is_init = true;
	}

	return channel_ptr;
}

enum DHALL_RETCODE dhall_is_present(dhall_t * handle) {
	/* Check for a valid handle: */
	if(!dhall_initialized) return DHALL_ERR_BADINIT;
	if(!handle)            return DHALL_ERR_BADHANDLE;
	if(!handle->is_init)   return DHALL_ERR_BADINIT;
	if(!handle->signalpin || !handle->cback || handle->mode == DHALL_MODE_NULL || handle->mode >= DHALL_MODE__COUNT) return DHALL_ERR_BADARGS;

	/* Read the pin and return its input value: */
	return R_GPIO_PinRead(handle->signalpin) == GPIO_LEVEL_HIGH ? DHALL_NOTDETECTED : DHALL_DETECTED;
}
#endif
