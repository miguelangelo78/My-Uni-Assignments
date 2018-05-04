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
	uint8_t  getset;        /* 0: Get data. 1: Set data    */
	uint8_t  dev_id;        /* Device ID                   */
	uint8_t  operation[10]; /* The command itself          */
	uint32_t size;          /* Size of the command payload */
} packet_cmd_t;

#endif /* LIB_DRIVERS_COMMUNICATIONS_PROTOCOLS_PACKETMAN_PACKET_TYPES_H_ */
