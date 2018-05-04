/*
 * stdio_redirect.c
 *
 *  Created on: 28/10/2017
 *      Author: Miguel
 */

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <data_structures/fifo.h>
#include <rtos_inc.h>
#include <drivers/communications/protocols/suart/suart.h>
#include <drivers/onchip/led.h>
#include <bootloader/bootloader.h>
#include <app_config.h>

suart_t * module_iostream; /* Pointer to the software UART module handle */
bool stdio_is_init = false;
volatile fifo_t * stdin_fifo = NULL; /* We need a fifo for the stdin so that the SUART dispatcher can push data into it */

void stdio_init(suart_t * iostream_device) {
	if(stdio_is_init || !iostream_device)
		return;

	module_iostream = iostream_device;

	/* Let the suart protocol and the read_fifo handle the buffering */
	setbuf(stdout, NULL);
	/* Create a FIFO buffer for stdin */
	stdin_fifo = fifo_create(module_iostream->rx_fifo->bufflen);
	stdio_is_init = true;
}

int write(int fd, const void * buf, size_t count) {
	if(!module_iostream || !buf || !count)
		return -1;

	/* How many bytes have been written */
	int txcount = 0;

	/* Write the buffer out through the Bluetooth module */
	for(int i = 0; i < count; i += module_iostream->tx_fifo->bufflen) {

		/* How many bytes from the input buffer fit on the Bluetooth module's driver internal buffer */
		int txlen = count - txcount > module_iostream->tx_fifo->bufflen ? module_iostream->tx_fifo->bufflen : count - i;
		txcount += txlen;

		/* Transmit the data */
		if(suart_tx_buff_sync(module_iostream, (uint8_t*)buf + i, txlen) != SUART_OK)
			return txcount;
	}

	return txcount;
}

int read(int fd, const void *buf, size_t count) {
	if(!module_iostream || !stdin_fifo || !buf || !count || count > stdin_fifo->bufflen)
		return -1;

	/* Wait until we get enough amount of bytes */
	while(stdin_fifo->buff_fifo_sz < count)
		rtos_preempt();

	/* First, quickly copy incoming buffer */
	for(int i = 0; i < count; i++)
		((uint8_t*)buf)[i] = stdin_fifo->buff[i + stdin_fifo->buff_fifo_idx];

	stdin_fifo->buff_fifo_idx += count;
	if(stdin_fifo->buff_fifo_idx == stdin_fifo->buff_fifo_sz)
		fifo_flush((fifo_t*)stdin_fifo);

	/* And now echo the received data back into stdout */
	for(int i = 0; i < count; i++) {
		if(!is_bootloader_busy) /* (Don't want to use the LEDs if the MCU is flashing its own ROM) */
			debug_leds_update(1);

		putchar(((uint8_t*)buf)[i]);

		if(!is_bootloader_busy) /* (Don't want to use the LEDs if the MCU is flashing its own ROM) */
			debug_leds_reset(1);
	}

	return count;
}

int stdin_push_buffer(int fd, const void* buf, size_t count) {
	if(!module_iostream || !stdin_fifo || !buf || !count || count > stdin_fifo->bufflen)
		return -1;

	/* Push buffer into stdin fifo buffer */
	if(fifo_push((fifo_t*)stdin_fifo, (uint8_t*)buf, count) != FIFO_OK)
		return -1;

	return count;
}

int stdin_available(void) {
	return stdin_fifo->buff_fifo_sz;
}
