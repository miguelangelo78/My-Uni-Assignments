/*
 * packet_types.h
 *
 *  Created on: 27/12/2017
 *      Author: Miguel
 */

#ifndef LIB_DRIVERS_COMMUNICATIONS_PROTOCOLS_PACKETMAN_PACKET_TYPES_H_
#define LIB_DRIVERS_COMMUNICATIONS_PROTOCOLS_PACKETMAN_PACKET_TYPES_H_

#include <stdbool.h>

typedef struct {uint8_t nil;} packet_null_t;

typedef struct {
	bool connRequest;
	bool connAck;
} packet_connect_t;

typedef struct {
	bool disconnRequest;
	bool disconnAck;
} packet_disconnect_t;

typedef struct {
	bool iAmAlive;
} packet_keepalive_t;

typedef struct {
	/* TODO */
	uint8_t nil;
} packet_cmd_t;

typedef struct {
	/* TODO */
	uint8_t nil;
} packet_stream_t;

#endif /* LIB_DRIVERS_COMMUNICATIONS_PROTOCOLS_PACKETMAN_PACKET_TYPES_H_ */
