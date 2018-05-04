/*
 * vs1838b.h
 *
 *  Created on: 24/02/2017
 *      Author: Miguel
 */

#ifndef DRIVERS_COMMUNICATIONS_INFRARED_VS1838B_H_
#define DRIVERS_COMMUNICATIONS_INFRARED_VS1838B_H_

#include <stdbool.h>
#include <libs/data_structures/fifo.h>

#define IRREM_MAX_CHANNEL_COUNT    1
#define IRREM_RX_AUTOFLUSH_ENABLE (1) /* Enable feature: after an 'X' amount of poll cycles, if no bytes are received then we will dispatch and flush RX buffer */
#define IRREM_RX_AUTOFLUSH_COUNTER 3  /* How many cycles will we wait before timing out / dispatching + flushing out the RX buffer */
#define NEC_MAX_ATOMIC_RECEPTION  128 /* How many '562.5uS' blocks do we need to sample */

typedef void(*irrem_rx_cback_t)(uint16_t * buff, uint32_t bufflen);

enum IRREM_ERRCODE {
	IRREM_OK,
	IRREM_ERR_BADINIT,
	IRREM_ERR_RXBUSY,
	IRREM_ERR_RX_FIFO_FULL,
	IRREM_ERR_BAD_RX_DISPATCH,
	IRREM_ERR__COUNT /* How many different types of errors there are */
};

typedef struct {
	int8_t leading_pulse1;
	int8_t leading_pulse2;
	int8_t space;
	int8_t payload[12];
} nec_template_t;

typedef struct {
	int8_t address;
	int8_t command;
} nec_packet_t;

typedef struct {
	bool             is_init;
	irrem_rx_cback_t rx_cback;
	gpio_port_pin_t  rxpin;
	bool             rxing;
	/* RX FIFO Buffer: */
	fifo_t         * rx_fifo;
	uint32_t         rx_prescale_polls;
	uint32_t         rx_delta_polls;
	int8_t           rx_timeout;
	uint16_t         previous_short;
	nec_template_t   sampling_data;
	uint32_t         sampling_counter;
} irrem_t;

void irrem_poll(void);
irrem_t * irrem_init(gpio_port_pin_t rxpin, irrem_rx_cback_t rx_cback, uint32_t rx_bufflen);

#endif /* DRIVERS_COMMUNICATIONS_INFRARED_VS1838B_H_ */
