/*
 * suart.c
 *
 *  Created on: 15/10/2017
 *      Author: Miguel
 */

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <rtos_wrappers/rtos_new_lib.h>
#include <communications/protocols/packetman/packetman.h>
#include <communications/protocols/suart/suart.h>
#include <rtos/Micrium/uCOS-III/Source/os_cfg_app.h>
#include <onchip/led.h>
#include <bootloader/bootloader.h>
#include <app_config.h>

#if (1) /**** REGION: SUART DRIVER PRIVATE IMPLEMENTATION ****/
/************************************/
/****** PRIVATE IMPLEMENTATION ******/
/************************************/

#ifdef APP_CFG_POLLFREQ
#define BAUD_TO_PRESCALER(baudrate) (uint32_t)(APP_CFG_POLLFREQ / (baudrate))
#else
#define BAUD_TO_PRESCALER(baudrate) (uint32_t)(OS_CFG_TICK_RATE_HZ / (baudrate))
#endif

typedef struct {
	uint8_t * buff;
	uint32_t  bufflen;
	bool      dispatch_this; /* If 1 then the packet is ready to be dispatched */
} suart_rx_cback_dispatch_packet_channel_t;

bool     suart_initialized   = false;
uint32_t suart_alloc_channel = 0;
volatile suart_t suart_channels[SUART_MAX_CHANNEL_COUNT];
volatile suart_rx_cback_dispatch_packet_channel_t suart_dispatch_channels[SUART_MAX_CHANNEL_COUNT];

enum SUART_CHANNEL_WAY {
	SUART_WAY_TX,
	SUART_WAY_RX
};

/* Function Prototype: */
uint8_t suart_get_channel_idx(suart_t * handle);

bool is_baud_respected(suart_t * handle, enum SUART_CHANNEL_WAY way) {
	if(way == SUART_WAY_TX) {
		if(++handle->tx_prescale_polls < handle->baud_prescaler) return false;
		handle->tx_prescale_polls = 0;
	} else if(way == SUART_WAY_RX) {
		if(++handle->rx_prescale_polls < handle->baud_prescaler) return false;
		handle->rx_prescale_polls = 0;
	}
	return true;
}

enum SUART_ERRCODE suart_rx_callback_dispatch(suart_t * handle, uint8_t * buff, uint32_t bufflen) {
	uint8_t channel = suart_get_channel_idx(handle);
	if(channel == (uint8_t)-1) return SUART_ERR_BAD_RX_DISPATCH;

	memcpy(suart_dispatch_channels[channel].buff, buff, bufflen);
	suart_dispatch_channels[channel].bufflen = bufflen;
	suart_dispatch_channels[channel].dispatch_this = true; /* Dispatch this RX packet! */
	return SUART_OK;
}

void suart_rx_poll(suart_t * chan) {
#if SUART_FULLDUPLEX == 0
	if(chan->txing) return;
#endif

	if(chan->rxing) {
		/* Respect the BAUD timings: */
		if(!is_baud_respected(chan, SUART_WAY_RX)) return;

		switch(chan->rx_delta_polls) {
		case 8: /* Receive stop bit */
			chan->rx_delta_polls = 0;
			chan->rxing = false;

			if(DAT_SUART_RX) {
				/* We found a stop bit */
#if SUART_ENABLE_NEWLINE_TERMINATOR == 1
				if(chan->rx_fifo->buff_fifo_idx >= chan->rx_fifo->bufflen - 1 || chan->rx_fifo->buff[chan->rx_fifo->buff_fifo_idx] == SUART_NEWLINE_CHARACTER) {
#else
				if(chan->rx_buff_fifo_idx >= chan->rx_bufflen - 1) {
#endif
					/* The FIFO buffer is now full. We're done with the reception */
					if(chan->rx_cback)
						suart_rx_callback_dispatch(chan, chan->rx_fifo->buff, chan->rx_fifo->buff_fifo_idx);
					fifo_flush(chan->rx_fifo);
				} else {
					/* Receive next byte into the FIFO queue / buffer */
					fifo_push_byte(chan->rx_fifo, chan->rx_fifo->buff[chan->rx_fifo->buff_fifo_idx]);
					chan->rx_fifo->buff_fifo_idx++;
				}
			} else {
				/* We didn't find a stop bit... Reception byte must be dropped */
				chan->rx_fifo->buff[chan->rx_fifo->buff_fifo_idx] = 0;
			}
			break;

		default: {
				uint8_t bit = DAT_SUART_RX;
				chan->rx_fifo->buff[chan->rx_fifo->buff_fifo_idx] |= (bit << chan->rx_delta_polls);
				chan->rx_delta_polls++;
			}
			break;
		}
	} else {
		/* Check for start bit: */
		if(!DAT_SUART_RX) {
			/* We've received a start bit! */
			chan->rxing = true;
#if SUART_RX_AUTOFLUSH_ENABLE == 1
			chan->rx_timeout = SUART_RX_AUTOFLUSH_COUNTER;
#endif
		} else {
			/* Nothing is coming through */
#if SUART_RX_AUTOFLUSH_ENABLE == 1
			if(chan->rx_timeout > 0) {
				chan->rx_timeout--;
			} else {
				/* Reception timed out. We won't wait for the buffer to be full in order to dispatch it to the callback. Just dispatch it now */
				if(chan->rx_cback && chan->rx_fifo->buff_fifo_sz) {
					suart_rx_callback_dispatch(chan, chan->rx_fifo->buff, chan->rx_fifo->buff_fifo_idx);
					fifo_flush(chan->rx_fifo);
				}
				chan->rx_timeout = SUART_RX_AUTOFLUSH_COUNTER;
			}
#endif
		}
	}
}

void suart_tx_poll(suart_t * chan) {
#if SUART_FULLDUPLEX == 0
	if(chan->rxing) return;
#endif

	if(chan->txing) {
		/* Respect the BAUD timings: */
		if(!is_baud_respected(chan, SUART_WAY_TX)) return;

		/* Send single bit: */
		switch(chan->tx_delta_polls) {
		case 0: /* Send start bit */
			DAT_SUART_TX = 0;
			chan->tx_delta_polls++;
			if(!is_bootloader_busy && packetman_get_comms_status() <= STREAM_BYTE)
				debug_leds_update(0);
			break;
		case 9: /* Send stop bit and finish transmission */
			DAT_SUART_TX = 1;
			chan->tx_delta_polls = 0;
			if(chan->tx_fifo->buff_fifo_idx >= chan->tx_fifo->buff_fifo_sz - 1) {
				/* Done transmitting the entire FIFO queue / buffer */
				chan->tx_fifo->buff_fifo_idx = 0;
				chan->tx_fifo->buff_fifo_sz  = 0;
				chan->txing = false;
			} else {
				/* Send next byte on the FIFO queue / buffer */
				chan->tx_fifo->buff_fifo_idx++;
			}
			if(!is_bootloader_busy && packetman_get_comms_status() <= STREAM_BYTE)
				debug_leds_reset(0);
			break;
		default: /* Send 'nth' data bit */
			DAT_SUART_TX = (chan->tx_fifo->buff[chan->tx_fifo->buff_fifo_idx] & (1 << (chan->tx_delta_polls - 1))) ? 1 : 0;
			chan->tx_delta_polls++;
			break;
		}
	}
}

uint8_t suart_get_channel_idx(suart_t * handle) {
	if(!handle) return (uint8_t)-1;
	for(uint8_t i = 0; i < SUART_MAX_CHANNEL_COUNT; i++)
		if(handle == &suart_channels[i])
			return i;
	return (uint8_t)-1;
}

bool suart_dispatcher_needs_to_be_dispatched(suart_t * channel, uint8_t channel_idx) {
	return channel && channel->baudrate && channel->is_init && channel->rx_cback && suart_dispatch_channels[channel_idx].dispatch_this;
}

void suart_rx_callback_dispatcher(void * args) {
	suart_t * channel   = (suart_t*)args;
	uint8_t channel_idx = suart_get_channel_idx(channel);

	while(1) {
		if(suart_dispatcher_needs_to_be_dispatched(channel, channel_idx)) {
			channel->rx_cback(suart_dispatch_channels[channel_idx].buff, suart_dispatch_channels[channel_idx].bufflen);
			suart_dispatch_channels[channel_idx].dispatch_this = false;
		}
		rtos_preempt();
	}
}

void suart_init_all_channels(void) {
	for(uint8_t i = 0; i < SUART_MAX_CHANNEL_COUNT; i++) {
		/* Initialize software UART struct variables: */
		suart_channels[i].baudrate          = 0;
		suart_channels[i].baud_prescaler    = 0;
		suart_channels[i].tx_fifo           = 0;
		suart_channels[i].rx_fifo           = 0;
		suart_channels[i].rxing             = false;
		suart_channels[i].txing             = false;
		suart_channels[i].tx_delta_polls    = 0;
		suart_channels[i].rx_delta_polls    = 0;
		suart_channels[i].tx_prescale_polls = 0;
		suart_channels[i].rx_prescale_polls = 0;
		suart_channels[i].rx_cback          = 0;
#if SUART_RX_AUTOFLUSH_ENABLE == 1
		suart_channels[i].rx_timeout        = SUART_RX_AUTOFLUSH_COUNTER;
#else
		suart_channels[i].rx_timeout        = -1;
#endif
		suart_channels[i].is_init           = false;

		/* Initialize RX callback channels: */
		suart_dispatch_channels[i].buff          = 0;
		suart_dispatch_channels[i].bufflen       = 0;
		suart_dispatch_channels[i].dispatch_this = 0;
	}
}

#endif

#if (1) /**** REGION: SUART DRIVER PUBLIC IMPLEMENTATION ****/
/***********************************/
/****** PUBLIC IMPLEMENTATION ******/
/***********************************/
void suart_poll(void) {
#if SUART_ENABLE_TX || SUART_ENABLE_RX
	for(uint8_t i = 0; i < SUART_MAX_CHANNEL_COUNT; i++) {
		suart_t * chan = (suart_t*)&suart_channels[i];
		if(chan->baudrate && chan->is_init) {
#if SUART_ENABLE_TX
			suart_tx_poll(chan);
#endif
#if SUART_ENABLE_RX
			suart_rx_poll(chan);
#endif
		}
	}
#endif
}

enum SUART_ERRCODE suart_tx(suart_t * handle, uint8_t byte) {
	if(!suart_initialized) return SUART_ERR_BADINIT;
	uint8_t channel = suart_get_channel_idx(handle);
	if(channel == (uint8_t)-1) return SUART_ERR_BADTX;

	/* Wait for RX / TX to finish: */
#if SUART_FULLDUPLEX == 0
	if(suart_channels[channel].rxing) return SUART_ERR_RXBUSY;
#endif

	if(fifo_is_full(handle->tx_fifo)) return SUART_ERR_TX_FIFO_FULL;

	/* Push byte Asynchronously: */
	fifo_push_byte(suart_channels[channel].tx_fifo, byte);
	suart_channels[channel].txing = true; /* Start transmission! */
	return SUART_OK;
}

enum SUART_ERRCODE suart_tx_sync(suart_t * handle, uint8_t byte) {
	enum SUART_ERRCODE success = suart_tx(handle, byte);
	if(success == SUART_OK)
		while(handle->txing)
			rtos_preempt();
	return success;
}

enum SUART_ERRCODE suart_tx_buff(suart_t * handle, uint8_t * buff, uint32_t length) {
	if(!suart_initialized) return SUART_ERR_BADINIT;
	uint8_t channel = suart_get_channel_idx(handle);
	if(channel == (uint8_t)-1) return SUART_ERR_BADTX;

	/* Wait for RX / TX to finish: */
#if SUART_FULLDUPLEX == 0
	if(suart_channels[channel].rxing) return SUART_ERR_RXBUSY;
#endif

	if(fifo_is_full(handle->tx_fifo)) return SUART_ERR_TX_FIFO_FULL;

	/* Push buffer Asynchronously: */
	if(fifo_push(suart_channels[channel].tx_fifo, buff, length) == SUART_ERR_TX_FIFO_FULL)
		return SUART_ERR_TX_FIFO_FULL;

	/* Start transmission! */
	suart_channels[channel].txing = true;
	return SUART_OK;
}

enum SUART_ERRCODE suart_tx_buff_sync(suart_t * handle, uint8_t * buff, uint32_t length) {
	enum SUART_ERRCODE success;

	while((success = suart_tx_buff(handle, buff, length)) == SUART_ERR_TX_FIFO_FULL)
		rtos_preempt();

	if(success == SUART_OK)
		while(handle->txing)
			rtos_preempt();
	return success;
}

enum SUART_ERRCODE suart_printstr(suart_t * handle, char * str) {
	return suart_tx_buff_sync(handle, (uint8_t*)str, strlen(str));
}

void stdio_init(suart_t * iostream_device);

suart_t * suart_init(uint32_t baudrate, suart_rx_cback_t rx_cback_ptr, uint32_t tx_bufflen, uint32_t rx_bufflen) {
	if(baudrate == 0 || baudrate > SUART_MAX_BAUD || suart_alloc_channel >= SUART_MAX_CHANNEL_COUNT) return (suart_t*)SUART_ERR_BADINIT;

	suart_t * channel_ptr = (suart_t*)SUART_ERR_BADINIT;

	/* Only initialize the new channel if the buffer length is larger than 0: */
	if(tx_bufflen > 0 || rx_bufflen > 0) {
		if(!suart_initialized) {
			suart_init_all_channels();
			suart_initialized = true;
		}

		/* Initialize channel's variables: */
		suart_channels[suart_alloc_channel].baudrate       = baudrate;
		suart_channels[suart_alloc_channel].baud_prescaler = BAUD_TO_PRESCALER(baudrate) + 1;
		if(tx_bufflen)
			suart_channels[suart_alloc_channel].tx_fifo    = fifo_create(tx_bufflen);
		if(rx_bufflen)
			suart_channels[suart_alloc_channel].rx_fifo    = fifo_create(rx_bufflen);
		suart_channels[suart_alloc_channel].rxing          = false;
		suart_channels[suart_alloc_channel].txing          = false;
		suart_channels[suart_alloc_channel].rx_cback       = rx_cback_ptr;

		/* Initialize RX callback channels: */
		if(rx_bufflen)
			suart_dispatch_channels[suart_alloc_channel].buff = (uint8_t*)malloc(rx_bufflen);
		suart_dispatch_channels[suart_alloc_channel].bufflen  = rx_bufflen;

		/* Initialize RX and TX pins: */
		DIR_SUART_RX = 0;
		DIR_SUART_TX = 1;
		DAT_SUART_TX = 1;

		/* Fetch this channel's address: */
		channel_ptr = (suart_t*)&suart_channels[suart_alloc_channel];

		/* Spawn RX callback dispatcher for this particular channel: */
		if(rx_cback_ptr)
			rtos_spawn_task_args("suart_rx_cbk_dspchr", suart_rx_callback_dispatcher, channel_ptr);

		/* Make next channel available for allocation: */
		suart_channels[suart_alloc_channel++].is_init = true;

		/* Initialize stdio */
		stdio_init(channel_ptr);
	}

	return channel_ptr;
}
#endif
