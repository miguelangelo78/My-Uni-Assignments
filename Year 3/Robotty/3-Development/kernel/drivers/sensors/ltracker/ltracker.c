/*
 * ltracker.c
 *
 *  Created on: 08/03/2017
 *      Author: Miguel
 */

#include <drivers/onchip/r_gpio_rx/r_gpio_rx_if.h>
#include <globals.h>

#if (1) /**** REGION: LTRACKER DRIVER PRIVATE IMPLEMENTATION ****/
/************************************/
/****** PRIVATE IMPLEMENTATION ******/
/************************************/
bool     ltracker_initialized   = false;
uint32_t ltracker_alloc_channel = 0;
volatile ltracker_t ltracker_channels[LTRACKER_MAX_CHANNEL_COUNT];

void lrtracker_update_pin(ltracker_pin_t * tracker_pin) {
	tracker_pin->bright = R_GPIO_PinRead(tracker_pin->pin) == GPIO_LEVEL_HIGH;
}

void ltracker_poller(void * args) {
	ltracker_t * channel = (ltracker_t*)args;

	while(1) {
		ltracker_line_t old_line = ltracker_get_line(channel);
		lrtracker_update_pin(&channel->lpin);
		lrtracker_update_pin(&channel->cpin);
		lrtracker_update_pin(&channel->rpin);
		ltracker_line_t new_line = ltracker_get_line(channel);

		/* Check if at least ONE of the lines has changed: */
		if(channel->onchange_callback != NULL_CBACK &&
			(old_line.left != new_line.left || old_line.center != new_line.center || old_line.right != new_line.right))
		{
			uint8_t which_line_changed = 0;

			/* If so, which line? */
			if(old_line.left != new_line.left)
				which_line_changed |= LTRACKER_LINEMASK_LEFT;
			if(old_line.center != new_line.center)
				which_line_changed |= LTRACKER_LINEMASK_CENTER;
			if(old_line.right != new_line.right)
				which_line_changed |= LTRACKER_LINEMASK_RIGHT;

			/* Call the callback 'onchange': */
			channel->onchange_callback(channel, which_line_changed);
		}

		rtos_preempt();
	}
}

void ltracker_init_all_channels(void) {
	for(uint8_t i = 0; i < LTRACKER_MAX_CHANNEL_COUNT; i++) {
		ltracker_channels[i].lpin.pin           = (gpio_port_pin_t)0;
		ltracker_channels[i].lpin.bright        = false;
		ltracker_channels[i].cpin.pin           = (gpio_port_pin_t)0;
		ltracker_channels[i].cpin.bright        = false;
		ltracker_channels[i].rpin.pin           = (gpio_port_pin_t)0;
		ltracker_channels[i].rpin.bright        = false;
		ltracker_channels[i].onchange_callback  = NULL_CBACK;
		ltracker_channels[i].mode               = LTRACKER_MODE_NULL;
		ltracker_channels[i].is_init            = false;
	}
}
#endif

#if (1) /**** REGION: LTRACKER DRIVER PUBLIC IMPLEMENTATION ****/
/***********************************/
/****** PUBLIC IMPLEMENTATION ******/
/***********************************/
ltracker_t * ltracker_init(gpio_port_pin_t leftpin, gpio_port_pin_t centerpin, gpio_port_pin_t rightpin, ltracker_on_change_t onchange_callback, enum LTRACKER_MODE mode) {
	if(ltracker_alloc_channel >= LTRACKER_MAX_CHANNEL_COUNT) return (ltracker_t*)LTRACKER_ERR_BADINIT;

	ltracker_t * channel_ptr = (ltracker_t*)LTRACKER_ERR_BADINIT;

	/* Only initialize the new channel if the input pins are non-zero and the mode is valid (the callback can be null): */
	if(leftpin && centerpin && rightpin && mode > LTRACKER_MODE_NULL && mode < LTRACKER_MODE__COUNT) {
		if(!ltracker_initialized) {
			ltracker_init_all_channels();
			ltracker_initialized = true;
		}

		/* Initialize channel's variables: */
		ltracker_channels[ltracker_alloc_channel].lpin.pin          = leftpin;
		ltracker_channels[ltracker_alloc_channel].cpin.pin          = centerpin;
		ltracker_channels[ltracker_alloc_channel].rpin.pin          = rightpin;
		ltracker_channels[ltracker_alloc_channel].onchange_callback = onchange_callback;
		ltracker_channels[ltracker_alloc_channel].mode              = mode;

		/* Initialize pins: */
		R_GPIO_PinDirectionSet(leftpin,   GPIO_DIRECTION_INPUT);
		R_GPIO_PinDirectionSet(centerpin, GPIO_DIRECTION_INPUT);
		R_GPIO_PinDirectionSet(rightpin,  GPIO_DIRECTION_INPUT);

		/* Fetch this channel's address: */
		channel_ptr = (ltracker_t*)&ltracker_channels[ltracker_alloc_channel];

		/* Create RTOS task for polling the 3 signal pins: */
		spawn_task_args("ltracker_poller", ltracker_poller, channel_ptr);

		/* Make next channel available for allocation: */
		ltracker_channels[ltracker_alloc_channel++].is_init = true;
	}

	return channel_ptr;
}

ltracker_line_t ltracker_get_line(ltracker_t * handle) {
	ltracker_line_t line;

	/* Initialize the line variable: */
	line.left = line.center = line.right = line.inval = false;

	/* Check for a valid handle: */
	if(!ltracker_initialized || !handle || !handle->is_init || !handle->lpin.pin || !handle->cpin.pin || !handle->rpin.pin ||
			handle->mode == LTRACKER_MODE_NULL || handle->mode >= LTRACKER_MODE__COUNT) {
		line.inval = true;
		return line;
	}

	if(!handle->lpin.bright && !handle->cpin.bright && !handle->rpin.bright) {
		/* All 3 IR sensors are off. If we are in white line mode, then no line is being detected. */
		if(handle->mode == LTRACKER_MODE_LINE_WHITE) {
			return line;
		} else {
			/* Else, if we're in black line mode and all IR sensors are off, then we either detected a very "thick" black line,
			 * or the sensors were lifted off the floor. This can only be determined by the accelerometer and GPS, as they
			 * could give information about the Y position of the sensor (since it's impossible to distinguish between a
			 * very wide black line and a sensor not being close to the floor) */
			line.left = line.center = line.right = true;
			return line;
		}
	} else {
		/* At least ONE of the sensors detected a white line */
		if(handle->mode == LTRACKER_MODE_LINE_WHITE) {
			/* In white line mode, if the boolean 'bright' is true, then the line was detected */
			line.left   = handle->lpin.bright;
			line.center = handle->cpin.bright;
			line.right  = handle->rpin.bright;
		} else {
			/* In black line mode, if the boolean 'bright' is true, then the line was NOT detected, therefore,
			 * the logic must be inverted */
			line.left   = !handle->lpin.bright;
			line.center = !handle->cpin.bright;
			line.right  = !handle->rpin.bright;
		}
	}

	return line;
}
#endif
