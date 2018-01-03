/*
 * vs1838B.c
 *
 *  Created on: 24/02/2017
 *      Author: Miguel
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <drivers/onchip/r_gpio_rx/r_gpio_rx_if.h>
#include <system/rtos_wrappers/rtos_new_lib.h>
#include "vs1838b.h"

#if(1) /**** REGION: VS1838B DRIVER PRIVATE IMPLEMENTATION ****/
/************************************/
/****** PRIVATE IMPLEMENTATION ******/
/************************************/
#define INFRARED_POLLING_PRESCALER 57 /* We'll sample the Infrared pin at this prescaler value (which depends strongly on the frequency of the polling frequency of the ISR) */

typedef struct {
	uint16_t * buff;
	uint32_t   bufflen;
	bool       dispatch_this; /* if 1 then the packet is ready to be dispatched */
} irrem_rx_cback_dispatch_packet_channel_t;

bool irrem_initialized = false;
uint32_t irrem_alloc_channel = 0;
volatile irrem_t irrem_channels[IRREM_MAX_CHANNEL_COUNT];
volatile irrem_rx_cback_dispatch_packet_channel_t irrem_dispatch_channels[IRREM_MAX_CHANNEL_COUNT];

/* Create NEC protocol template for fast matching with the sample: */
nec_template_t nec_template = {
	.leading_pulse1 = 0xFF,
	.leading_pulse2 = 0xFF,
	.space          = 0
};

enum IRREM_RECEPTION_TYPE {
	IRREM_RX_TYPE_NORMAL,
	IRREM_RX_TYPE_REPEAT_CODE,
	IRREM_RX_TYPE_INVAL,
	IRREM_RX_TYPE__COUNT /* How many different types of errors there are */
};

/* Function Prototype: */
uint8_t irrem_get_channel_idx(irrem_t * handle);

uint16_t irrem_convert_sample(irrem_t * chan) {
	uint16_t converted_value = 0;
	uint8_t  converted_value_bit_index = 0;
	nec_template_t * sample = &chan->sampling_data;
	uint8_t zero_counter = 0;
	bool start_counting = false;

	for(uint32_t i = 0; i < 70; i++) {
		uint8_t bitoff = i % 8;
		uint8_t bit = (sample->payload[(uint32_t)(i / 8)] & (1 << bitoff)) >> bitoff;

		if(bit == 0 && start_counting)
			zero_counter++;

		if(bit) {
			if(start_counting) {
				if(zero_counter == 1) {
					/* Found binary 0 */
				} else if(zero_counter == 3) {
					/* Found binary 1 */
					converted_value |= (1 << converted_value_bit_index);
				} else {
					/* Found an invalid pattern... We must not allow the conversion to continue */
					return (uint16_t)-1;
				}
				converted_value_bit_index++;
				zero_counter = 0;
			}
			start_counting = true;
		}

		/* Ignore NAddress field: */
		if(i == 15)
			i = 45;
	}
	return converted_value;
}

enum IRREM_RECEPTION_TYPE irrem_get_sample_type(irrem_t * chan) {
	enum IRREM_RECEPTION_TYPE type = IRREM_RX_TYPE_INVAL;
	nec_template_t * sample = &chan->sampling_data;

	/* Match the leading pulse: */
	if(sample->leading_pulse1 == nec_template.leading_pulse1 && sample->leading_pulse2 == nec_template.leading_pulse2)
		type = IRREM_RX_TYPE_NORMAL;
	else
		type = IRREM_RX_TYPE_INVAL;

	/* Match the repeat code: */
	if(sample->space == 16 && type == IRREM_RX_TYPE_NORMAL)
		/* This reception is a repeat code! Return this type now */
		return IRREM_RX_TYPE_REPEAT_CODE;

	/* Match the space: */
	if(sample->space != nec_template.space)
		type = IRREM_RX_TYPE_INVAL;

	/* Match the end: */
	if(!(sample->payload[11] & 4))
		type = IRREM_RX_TYPE_INVAL;

	return type;
}

enum IRREM_ERRCODE irrem_rx_callback_dispatch(irrem_t * handle, uint16_t * buff, uint32_t bufflen) {
	uint8_t channel = irrem_get_channel_idx(handle);
	if(channel == (uint8_t)-1) return IRREM_ERR_BAD_RX_DISPATCH;

	for(uint32_t i = 0; i < bufflen; i++)
		irrem_dispatch_channels[channel].buff[i] = buff[i];
	irrem_dispatch_channels[channel].bufflen = bufflen;
	irrem_dispatch_channels[channel].dispatch_this = true; /* Dispatch this RX packet! */
	return IRREM_OK;
}

bool irrem_sample_pin(irrem_t * chan) {
	if(!R_GPIO_PinRead(chan->rxpin)) {
		uint8_t * nec_byte_blob = (uint8_t*)&chan->sampling_data + ((uint32_t)(chan->sampling_counter / 8));
		uint8_t bit_index = chan->sampling_counter % 8;
		*nec_byte_blob |= (1 << bit_index);
	}
	chan->sampling_counter++;
	return chan->sampling_counter >= NEC_MAX_ATOMIC_RECEPTION;
}

bool irrem_is_frequency_respected(irrem_t * handle) {
	if(++handle->rx_prescale_polls < INFRARED_POLLING_PRESCALER) return false;
	handle->rx_prescale_polls = 0;
	return true;
}

void irrem_rx_poll(irrem_t * chan) {
	if(!irrem_is_frequency_respected(chan)) return;

	if(chan->rxing) {
		if(irrem_sample_pin(chan)) {
			/* Sampling has finished!
			 * Now we need to validate the sample, convert it and push it into the FIFO buffer */
			enum IRREM_RECEPTION_TYPE type = irrem_get_sample_type(chan);
			if(type == IRREM_RX_TYPE_NORMAL) {
				/* Received normal packet */
				uint16_t temp = irrem_convert_sample(chan);
				if(temp != (uint16_t)-1) {
					chan->previous_short = temp;
					fifo_push_short(chan->rx_fifo, chan->previous_short);
					chan->rx_fifo->buff_fifo_idx++;
					if(fifo_is_full(chan->rx_fifo)) {
						/* Dispatch FIFO buffer into RX callback: */
						if(chan->rx_cback)
							irrem_rx_callback_dispatch(chan, chan->rx_fifo->buff16, chan->rx_fifo->buff_fifo_idx);
						fifo_flush(chan->rx_fifo);
					}
				}
			} else if(type == IRREM_RX_TYPE_REPEAT_CODE) {
				/* Received a repeat code. Re-push the previously received packet */
				fifo_push_short(chan->rx_fifo, chan->previous_short);
				chan->rx_fifo->buff_fifo_idx++;
				if(fifo_is_full(chan->rx_fifo)) {
					/* Dispatch FIFO buffer into RX callback: */
					if(chan->rx_cback)
						irrem_rx_callback_dispatch(chan, chan->rx_fifo->buff16, chan->rx_fifo->buff_fifo_idx);
					fifo_flush(chan->rx_fifo);
				}
			} else if(type == IRREM_RX_TYPE_INVAL) {
				/* This packet is invalid. Dropping it. */
			}
			/* Clear up the sampling data: */
			memset((uint8_t*)&chan->sampling_data, 0, sizeof(nec_template_t));
			chan->sampling_counter = 0;
			chan->rxing = false;
		}
	} else {
		if(!R_GPIO_PinRead(chan->rxpin)) {
			/* The Infrared sensor is currently starting transmitting! */
			irrem_sample_pin(chan);
			chan->rxing = true;
#if IRREM_RX_AUTOFLUSH_ENABLE == 1
			chan->rx_timeout = IRREM_RX_AUTOFLUSH_COUNTER;
#endif
		} else {
			/* Nothing is coming through */
#if IRREM_RX_AUTOFLUSH_ENABLE == 1
			if(chan->rx_timeout > 0) {
				chan->rx_timeout--;
			} else {
				/* Reception timed out. We won't wait for the buffer to be full in order to dispatch it to the callback. Just dispatch it now */
				if(chan->rx_cback && chan->rx_fifo->buff_fifo_sz) {
					irrem_rx_callback_dispatch(chan, chan->rx_fifo->buff16, chan->rx_fifo->buff_fifo_idx);
					fifo_flush(chan->rx_fifo);
				}
				chan->rx_timeout = IRREM_RX_AUTOFLUSH_COUNTER;
			}
#endif
		}
	}
}

uint8_t irrem_get_channel_idx(irrem_t * handle) {
	if(!handle) return (uint8_t)-1;
	for(uint8_t i = 0; i < IRREM_MAX_CHANNEL_COUNT; i++)
		if(handle == &irrem_channels[i])
			return i;
	return (uint8_t)-1;
}

bool irrem_dispatcher_needs_to_be_dispatched(irrem_t * channel, uint8_t channel_idx) {
	return channel && channel->is_init && channel->rx_cback && irrem_dispatch_channels[channel_idx].dispatch_this;
}

void irrem_rx_callback_dispatcher(void * args) {
	irrem_t * channel   = (irrem_t*)args;
	uint8_t channel_idx = irrem_get_channel_idx(channel);

	while(1) {
		if(irrem_dispatcher_needs_to_be_dispatched(channel, channel_idx)) {
			channel->rx_cback(irrem_dispatch_channels[channel_idx].buff, irrem_dispatch_channels[channel_idx].bufflen);
			irrem_dispatch_channels[channel_idx].dispatch_this = false;
		}
		rtos_preempt();
	}
}


void irrem_init_all_channels(void) {
	for(uint8_t i = 0; i < IRREM_MAX_CHANNEL_COUNT; i++) {
		/* Initialize IR Remote struct variables: */
		irrem_channels[i].rx_fifo           = 0;
		irrem_channels[i].rx_cback          = 0;
		irrem_channels[i].rxing             = false;
		irrem_channels[i].rxpin             = (gpio_port_pin_t)0;
		irrem_channels[i].rx_prescale_polls = 0;
		irrem_channels[i].rx_delta_polls    = 0;
		irrem_channels[i].previous_short    = 0;
		irrem_channels[i].sampling_counter  = 0;
		memset((void*)&irrem_channels[i].sampling_data, 0, sizeof(nec_template_t));
#if IRREM_RX_AUTOFLUSH_ENABLE == 1
		irrem_channels[i].rx_timeout        = IRREM_RX_AUTOFLUSH_COUNTER;
#else
		irrem_channels[i].rx_timeout        = -1;
#endif
		irrem_channels[i].is_init           = false;
	}
}

#endif

#if(1) /**** REGION: VS1838B DRIVER PUBLIC IMPLEMENTATION ****/
/***********************************/
/****** PUBLIC IMPLEMENTATION ******/
/***********************************/
void irrem_poll(void) {
	/* Since this Infrared sensor is only a receiver,
	 * we'll treat this as a normal Simplex RX communication channel */

	for(uint8_t i = 0; i < IRREM_MAX_CHANNEL_COUNT; i++) {
		irrem_t * chan = (irrem_t*)&irrem_channels[i];
		if(chan->is_init && chan->rx_cback)
			irrem_rx_poll(chan);
	}
}

irrem_t * irrem_init(gpio_port_pin_t rxpin, irrem_rx_cback_t rx_cback, uint32_t rx_bufflen) {
	if(irrem_alloc_channel >= IRREM_MAX_CHANNEL_COUNT) return (irrem_t*)IRREM_ERR_BADINIT;

	irrem_t * channel_ptr = (irrem_t*)IRREM_ERR_BADINIT;

	/* Only initialize the new channel if the buffer length is larger than 0: */
	if(rx_bufflen > 0) {
		if(!irrem_initialized) {
			irrem_init_all_channels();
			irrem_initialized = true;
		}

		/* Initialize channel's variables: */
		if(rx_bufflen)
			irrem_channels[irrem_alloc_channel].rx_fifo = fifo16_create(rx_bufflen);
		irrem_channels[irrem_alloc_channel].rx_cback = rx_cback;
		irrem_channels[irrem_alloc_channel].rxpin    = rxpin;

		/* Initialize RX callback channels: */
		if(rx_bufflen)
			irrem_dispatch_channels[irrem_alloc_channel].buff = (uint16_t*)malloc(sizeof(uint16_t) * rx_bufflen);
		irrem_dispatch_channels[irrem_alloc_channel].bufflen  = rx_bufflen;

		/* Initialize RX and TX pins: */
		if(rxpin)
			R_GPIO_PinDirectionSet(rxpin, GPIO_DIRECTION_INPUT);

		/* Fetch this channel's address: */
		channel_ptr = (irrem_t*)&irrem_channels[irrem_alloc_channel];

		/* Spawn RX callback dispatcher for this particular channel: */
		spawn_task_args("irrem_rx_cbk_dspchr", irrem_rx_callback_dispatcher, channel_ptr);

		/* Make next channel available for allocation: */
		irrem_channels[irrem_alloc_channel++].is_init = true;
	}

	return channel_ptr;
}
#endif
