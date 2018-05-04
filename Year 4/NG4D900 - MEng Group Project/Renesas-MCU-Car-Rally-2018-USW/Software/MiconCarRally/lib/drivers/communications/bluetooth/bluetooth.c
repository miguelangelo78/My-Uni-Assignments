/*
 * bluetooth.c
 *
 *  Created on: 01/11/2017
 *      Author: Miguel
 */

#include <stddef.h>
#include "bluetooth.h"
#include <communications/protocols/suart/suart.h>
#include <communications/protocols/packetman/packetman.h>

suart_t * module_bluetooth;

void bluetooth_init(void) {
	/* Create and initialize HC-06 Bluetooth module */
	module_bluetooth = suart_init(
		PACKETMAN_BAUDRATE,
		packetman_on_bluetooth_rx,
		PACKETMAN_TXBUFFSIZE,
		PACKETMAN_RXBUFFSIZE
	);
}
