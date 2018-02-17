/*
 * suart.h
 *
 *  Created on: 15/10/2017
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_COMMUNICATIONS_PROTOCOLS_SUART_SUART_H_
#define LIB_DRIVERS_COMMUNICATIONS_PROTOCOLS_SUART_SUART_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <data_structures/fifo.h>

#define SUART_MAX_BAUD                  (9600) /* The maximum baud rate allowed                                                                                                                          */
#define SUART_MAX_CHANNEL_COUNT         (1)    /* How many channels shall we allocate for the Software UART protocol                                                                                     */
#define SUART_ENABLE_TX                 (1)    /* Forces disabling (0) / enabling (1) of the transmission functionality                                                                                  */
#define SUART_ENABLE_RX                 (1)    /* Forces disabling (0) / enabling (1) of the reception functionality                                                                                     */
#define SUART_FULLDUPLEX                (1)    /* If 0 then SUART works as Half-Duplex                                                                                                                   */
#define SUART_RX_AUTOFLUSH_ENABLE       (1)    /* Enable feature: after an 'X' amount of poll cycles, if no bytes are received and no newline character has been RX'd, then dispatch and flush RX buffer */
#define SUART_RX_AUTOFLUSH_COUNTER      (20)   /* How many cycles will we wait before timing out / dispatching + flushing out the RX buffer                                                              */
#define SUART_ENABLE_NEWLINE_TERMINATOR (1)    /* If 1, the reception of bytes terminates when a new line is found. This means the FIFO buffer won't NEED to be full for it to be re-flushed             */
#define SUART_NEWLINE_CHARACTER         ('\n') /* Which character is considered new line (could be '\r\n' or '\r' for example)                                                                           */

enum SUART_ERRCODE {
	SUART_OK,
	SUART_ERR_BADINIT,
	SUART_ERR_BADTX,
	SUART_ERR_TXBUSY,
	SUART_ERR_RXBUSY,
	SUART_ERR_TX_FIFO_FULL,
	SUART_ERR_RX_FIFO_FULL,
	SUART_ERR_BAD_RX_DISPATCH,
	SUART_ERR__COUNT /* How many different types of errors there are */
};

typedef void(*suart_rx_cback_t)(uint8_t * buff, uint32_t bufflen);

typedef struct {
	bool             is_init;
	uint32_t         baudrate;
	uint32_t         baud_prescaler;
	suart_rx_cback_t rx_cback;
	bool             rxing;
	bool             txing;
	/* TX FIFO Buffer: */
	fifo_t         * tx_fifo;
	/* RX FIFO Buffers: */
	fifo_t         * rx_fifo;
	int8_t           rx_timeout;
	uint32_t         tx_prescale_polls;
	uint32_t         rx_prescale_polls;
	uint8_t          tx_delta_polls;
	uint8_t          rx_delta_polls;
} suart_t;

enum SUART_ERRCODE suart_tx(suart_t * handle, uint8_t byte);
enum SUART_ERRCODE suart_tx_sync(suart_t * handle, uint8_t byte);
enum SUART_ERRCODE suart_tx_buff(suart_t * handle, uint8_t * buff, uint32_t bufflen);
enum SUART_ERRCODE suart_tx_buff_sync(suart_t * handle, uint8_t * buff, uint32_t bufflen);
enum SUART_ERRCODE suart_printstr(suart_t * handle, char * str);
enum SUART_ERRCODE suart_printf(suart_t * handle, char * fmt, ...);

#define suart_printch(handle, ch) suart_tx_sync(handle, ch)

void      suart_poll(void);
suart_t * suart_init(uint32_t baudrate, suart_rx_cback_t rx_cback_ptr, uint32_t tx_bufflen, uint32_t rx_bufflen);

int stdin_available(); /** Declared in <top>/lib/clib/stdio_redirect.c **/

#endif /* LIB_DRIVERS_COMMUNICATIONS_PROTOCOLS_SUART_SUART_H_ */
