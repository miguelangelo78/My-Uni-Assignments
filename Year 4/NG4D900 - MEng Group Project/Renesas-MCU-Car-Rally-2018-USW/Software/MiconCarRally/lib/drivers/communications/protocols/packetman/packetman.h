/*
 * packetman.h
 *
 *  Created on: 24/10/2017
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_COMMUNICATIONS_PROTOCOLS_PACKETMAN_PACKETMAN_H_
#define LIB_DRIVERS_COMMUNICATIONS_PROTOCOLS_PACKETMAN_PACKETMAN_H_

#include <stdint.h>

#define PERIOD_UPDATE                1          /* How frequent are the communications updated (in milliseconds)                           */
#define PERIOD_KEEPALIVE             1000       /* The period in which we expect to receive a keep alive packet                            */
#define STARTPACKET_MAGIC0           0xFEBAAFEB /* This MUST be received/transmitted in every start of packet                              */
#define STARTPACKET_MAGIC1           0x1BADB002 /* This MUST be received/transmitted in every start of packet after the first magic number */
#define PACKET_HEADER_SIZE           4 * 4      /* How large is the header of the packet (in bytes)                                        */
#define BYTESTREAM_TIMEOUT           50         /* How long shall we wait until we consider the byte stream reception to be over           */
#define BYTESTREAM_MAXSIZE           128        /* How large is the bytestream before we dispatch it into the data router                  */
#define BYTESTREAM_ENABLE_TERMINATOR (1)        /* Does the bytestream end + gets dispatched when it finds the character '\n' \ '\0'?      */
#define BYTESTREAM_TERMINATOR_CHAR   '\0'       /* What character terminates the byte stream buffer                                        */
#define RECONNECT_TRIES               3         /* How many tries shall we take to connect/reconnect to the car                            */
#define DISCONNECT_TRIES              3         /* How many tries shall we take to disconnect from the car                                 */
#define CONNECT_TIMEOUT               1000      /* How long shall we wait until the connect packet fails to acknowledge                    */
#define DISCONNECT_TIMEOUT            1000      /* How long shall we wait until the disconnect packet fails to acknowledge                 */
#define CONNECT_TRY_PERIOD            500       /* How long shall we wait in between every failed connect before we try again (ms)         */
#define KEEPALIVE_MAXCOUNTER          3         /* How many times will we allow the keepalive time out event to occur                      */

/* These defines are not necessarily part of the communication protocol */
#define PACKETMAN_RXBUFFSIZE 30   /* How large is the RX buffer (in bytes)                */
#define PACKETMAN_TXBUFFSIZE 64   /* How large is the TX buffer (in bytes)                */
#define PACKETMAN_BAUDRATE   9600 /* What baud rate shall we use for the Bluetooth module */

typedef struct {
	int    magic0;
	int    magic1;
	int    payloadSize;
	int    payloadType;
	char * payload;
} packet_t;

enum PacketType {
	PACKET_NULL,
	PACKET_CONNECT,
	PACKET_DISCONNECT,
	PACKET_KEEPALIVE,
	PACKET_CMD,
	PACKET__COUNT
};

enum STREAM_MODE {
	STREAM_NULL,
	STREAM_BYTE,
	STREAM_PACKET
};

void packetman_init(void);
void packetman_on_bluetooth_rx(uint8_t * buff, uint32_t bufflen);
void packetman_send_packet(void * payload_data, enum PacketType payload_type);
enum STREAM_MODE packetman_get_comms_status(void);
bool packetman_is_connected(void);
bool packetman_is_busy(void);

#endif /* LIB_DRIVERS_COMMUNICATIONS_PROTOCOLS_PACKETMAN_PACKETMAN_H_ */
