/*
 * spwm.c
 *
 *  Created on: 14/03/2017
 *      Author: Miguel
 */
#include <globals.h>
#include <drivers/onchip/r_gpio_rx/r_gpio_rx_if.h>

#if (1) /**** REGION: SPWM LIBRARY PRIVATE IMPLEMENTATION ****/
/************************************/
/****** PRIVATE IMPLEMENTATION ******/
/************************************/
#define SPWM_FREQ_TO_PRESC(hz) ((POLLFREQ) / (hz)) /* Get prescaler value from frequency */

bool     spwm_initialized   = false;
uint32_t spwm_alloc_channel = 0;
volatile spwm_t  spwm_channels[SPWM_MAX_CHANNEL_COUNT];
volatile uint8_t spwm_poll_idx = 0;

void spwm_poll_trigger(spwm_t * chan, gpio_level_t level) {
	uint8_t pinsample = (uint8_t)-1;
	if(chan->pin) {
		if(chan->mode & SPWM_MODE_PININPUT)
			pinsample = R_GPIO_PinRead(chan->pin);
		else
			R_GPIO_PinWrite(chan->pin, level);
	}

	if(chan->cback)
		chan->cback(pinsample);

	chan->level_latch = level == GPIO_LEVEL_HIGH ? true : false;
}

void spwm_poll_single_channel(spwm_t * chan) {
	if(chan->current_counter >= chan->match_counter) {
		/* (We are in the 'Low' level of the PWM wave) */

		if(chan->mode & SPWM_MODE_NEG) {
			if(chan->level_latch) {
				/* We are at the negative edge of the PWM wave. */
				spwm_poll_trigger(chan, GPIO_LEVEL_LOW);
			}
			else if(chan->mode & SPWM_MODE_REPEAT) {
				/* Already run this. We might want to repeat the trigger though. (Negative Level)  */
				spwm_poll_trigger(chan, GPIO_LEVEL_LOW);
			}
		} else {
			chan->level_latch = false;
		}

	} else {
		/* (We are in the 'High' level of the PWM wave) */

		if(chan->mode & SPWM_MODE_POS) {
			if(!chan->level_latch) {
				/* We are at the positive edge of the PWM wave. */
				spwm_poll_trigger(chan, GPIO_LEVEL_HIGH);
			}
			else if(chan->mode & SPWM_MODE_REPEAT) {
				/* Already run this. We might want to repeat the trigger though. (Positive Level)  */
				spwm_poll_trigger(chan, GPIO_LEVEL_HIGH);
			}
		} else {
			chan->level_latch = true;
		}
	}

	if(++chan->current_counter >= chan->max_counter)
		chan->current_counter = 0;
}

void spwm_poll(void) {
	spwm_t * chan = (spwm_t*)&spwm_channels[spwm_poll_idx];
	if(chan->is_init)
		spwm_poll_single_channel(chan);
	if(++spwm_poll_idx >= SPWM_MAX_CHANNEL_COUNT)
		spwm_poll_idx = 0;
}

void spwm_init_all_channels(void) {
	for(uint8_t i = 0; i < SPWM_MAX_CHANNEL_COUNT; i++) {
		spwm_channels[i].current_counter = 0;
		spwm_channels[i].match_counter   = 0;
		spwm_channels[i].max_counter     = 0;
		spwm_channels[i].duty_cycle      = 0;
		spwm_channels[i].mode            = SPWM_MODE_NULL;
		spwm_channels[i].cback           = 0;
		spwm_channels[i].pin             = (gpio_port_pin_t)0;
		spwm_channels[i].level_latch     = false;
		spwm_channels[i].is_init         = false;
	}
}
#endif

#if (1) /**** REGION: SPWM LIBRARY PUBLIC IMPLEMENTATION ****/
/***********************************/
/****** PUBLIC IMPLEMENTATION ******/
/***********************************/
spwm_t * spwm_create(uint32_t frequency_hz, uint8_t duty_cycle, uint8_t spwm_mode, spwm_cback_t cback, gpio_port_pin_t pin) {
	if(spwm_alloc_channel >= SPWM_MAX_CHANNEL_COUNT) return (spwm_t*)SPWM_ERR_BADINIT;

	spwm_t * channel_ptr = (spwm_t*)SPWM_ERR_BADINIT;

	/* Only initialize the new channel if the frequency and duty cycle are non-zero, if at least one of the trigger
	 * methods (cback or pin) is non-zero and the SPWM mode is valid. */

	bool valid_mode = (spwm_mode & 0x1F) && !((spwm_mode & SPWM_MODE_PININPUT) && !cback);
	if(!valid_mode)
		channel_ptr = (spwm_t*)SPWM_ERR_BADARGS;

	if(frequency_hz <= SPWM_MAX_FREQUENCY && duty_cycle <= 100 && (cback || pin) && valid_mode) {
		if(!spwm_initialized) {
			spwm_init_all_channels();
			spwm_initialized = true;
		}

		/* Initialize channel's variables: */
		spwm_channels[spwm_alloc_channel].pin           = pin;
		spwm_channels[spwm_alloc_channel].cback         = cback;
		spwm_channels[spwm_alloc_channel].max_counter   = SPWM_FREQ_TO_PRESC(frequency_hz);
		spwm_channels[spwm_alloc_channel].match_counter = (spwm_channels[spwm_alloc_channel].max_counter * duty_cycle) / 100;
		spwm_channels[spwm_alloc_channel].duty_cycle    = duty_cycle;
		spwm_channels[spwm_alloc_channel].mode          = spwm_mode;

		/* Initialize pin: */
		if(pin) {
			if(spwm_mode & SPWM_MODE_PININPUT) {
				R_GPIO_PinDirectionSet(pin, GPIO_DIRECTION_INPUT);
			} else {
				R_GPIO_PinDirectionSet(pin, GPIO_DIRECTION_OUTPUT);
				R_GPIO_PinWrite(pin, GPIO_LEVEL_HIGH);
			}
		}

		/* Fetch this channel's address: */
		channel_ptr = (spwm_t*)&spwm_channels[spwm_alloc_channel];

		/* Make next channel available for allocation: */
		spwm_channels[spwm_alloc_channel++].is_init = true;
	}

	return channel_ptr;
}

enum SPWM_ERR_CODE spwm_set_duty(spwm_t * handle, uint8_t duty_cycle) {
	if(!handle)          return SPWM_ERR_BADHANDLE;
	if(duty_cycle > 100) return SPWM_ERR_BADARGS;

	handle->match_counter   = (handle->max_counter * duty_cycle) / 100;
	handle->duty_cycle      = duty_cycle;
	handle->current_counter = 0;
	return SPWM_OK;
}

enum SPWM_ERR_CODE spwm_set_frequency(spwm_t * handle, uint32_t frequency_hz) {
	if(!handle)                           return SPWM_ERR_BADHANDLE;
	if(frequency_hz > SPWM_MAX_FREQUENCY) return SPWM_ERR_BADARGS;

	handle->max_counter     = SPWM_FREQ_TO_PRESC(frequency_hz);
	/* Recalculate match counter based on the new frequency: */
	handle->match_counter   = (handle->max_counter * handle->duty_cycle) / 100;
	handle->current_counter = 0;
	return SPWM_OK;
}
#endif
