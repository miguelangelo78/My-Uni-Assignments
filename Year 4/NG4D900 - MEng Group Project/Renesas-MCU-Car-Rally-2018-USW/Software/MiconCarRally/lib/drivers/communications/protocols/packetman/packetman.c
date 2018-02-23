/*
 * packetman.c
 *
 *  Created on: 24/10/2017
 *      Author: Miguel
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <rtos_inc.h>
#include <debug.h>
#include <clib/data_structures/fifo.h>
#include <communications/bluetooth/bluetooth.h>
#include <onchip/led.h>
#include <sound/piezo.h>
#include <sound/tunes.h>
#include <shell/shell.h>
#include "packetman.h"
#include "packet_types.h"
#include "data_packer.h"

enum STREAM_MODE stream_mode = STREAM_NULL;

int packet_type_sizes[] = {
	sizeof(packet_null_t),
	sizeof(packet_connect_t),
	sizeof(packet_disconnect_t),
	sizeof(packet_keepalive_t),
	sizeof(packet_cmd_t)
};

bool     is_packetman_init         = false;
bool     is_connected              = false;
bool     play_connected_tune       = false;
bool     play_disconnected_tune    = false;
bool     received_keepalive_packet = false;
int      keepalive_timeout_counter = 0;
bool     stream_byte_timeout       = false;
int      update_byte_stream_delta  = 0;
int      update_delta              = 0;
int      payload_size              = -1;
fifo_t * packet_byte_buffered      = NULL;

volatile bool packet_transmission_lock = false;

extern cmd_t command_list[];
extern int   command_count;

extern int stdin_push_buffer(int fd, const void* buf, size_t count);
extern int write(int fd, const void *buf, size_t count);

bool       is_packet(uint8_t * packet_bytes);
packet_t * create_packet(void * payload_data, uint32_t payload_length, enum PacketType payload_type);
void       discard_packet(packet_t * packet);

void reset_rx_buffer(bool clear_byte_buffer) {
	/* Reset this so we can receive a new packet of a different size */
	if(clear_byte_buffer)
		fifo_flush(packet_byte_buffered);
	payload_size = -1;
	stream_mode = STREAM_NULL;
}

void push_byte_stream(void) {
	/* Push received data into stdin */
	stdin_push_buffer(NULL, packet_byte_buffered->buff, packet_byte_buffered->buff_fifo_sz);
	reset_rx_buffer(true);
}

bool is_packet(uint8_t * packet_bytes) {
	/* Gather the data from the byte array */
	int magic0       = datapacker_get_int(packet_bytes, 0);
	int magic1       = datapacker_get_int(packet_bytes, 1);
	int payload_size = datapacker_get_int(packet_bytes, 2);
	int payload_type = datapacker_get_int(packet_bytes, 3);

	bool packet_valid  = magic0 == STARTPACKET_MAGIC0 && magic1 == STARTPACKET_MAGIC1;
	bool payload_valid = payload_type >= 0 && payload_type < PACKET__COUNT && payload_size == packet_type_sizes[payload_type];

	return packet_valid && payload_valid;
}

void packetman_on_bluetooth_rx(uint8_t * buff, uint32_t bufflen) {
	if(packet_byte_buffered == NULL) {
		/* Wait for Packet Man to initialize */
		return;
	}

	/* Push byte(s) into local packet byte buffer */
	if(fifo_push(packet_byte_buffered, buff, bufflen) == FIFO_ERR_FULL) {
		DEBUG("RX FIFO BUFFER IS FULL");
		stdin_push_buffer(NULL, buff, bufflen);
		return;
	}

	stream_byte_timeout = false;
	update_byte_stream_delta = 0;

	/* Interpret incoming buffer and decide where to send it
	 * (is it raw data (send to stdin) or packet (send it to packetman)) */
	if(packet_byte_buffered->buff_fifo_sz >= PACKET_HEADER_SIZE) {
		/* Received the minimum amount of bytes required to create a packet header.
		 * We must, however, check if this data is a packet or a normal stream of bytes */
		if(stream_mode == STREAM_BYTE) {
			/* Push received data into the normal byte stream */
			push_byte_stream();
			return;
		}

		if(payload_size == -1) {
			if(stream_mode == STREAM_NULL && !is_packet(packet_byte_buffered->buff)) {
				/* Not a packet! We must redirect this stream from now on to a normal byte stream handler */
				/* Push received byte stream */
				push_byte_stream();
				stream_mode = STREAM_BYTE;
				return;
			}

			/* It's a packet! Let's start */
			stream_mode = STREAM_PACKET;

			/* We've received at least the two magic numbers, the 32-bit word payloadSize and the payloadType.
			 * We now know how large the packet is */
			payload_size = datapacker_get_int(packet_byte_buffered->buff, 2);
		} else {
			/* Keep receiving the bytes until we reach the total size of the packet */
		}

		int total_packet_size = PACKET_HEADER_SIZE + payload_size;

		if(packet_byte_buffered->buff_fifo_sz >= total_packet_size) {
			/* We've received enough bytes.
			 * Time to parse and construct the packet properly */
			packet_t * rx_packet = create_packet(
				(void*)(packet_byte_buffered->buff + PACKET_HEADER_SIZE),
				payload_size,
				(enum PacketType)datapacker_get_int(packet_byte_buffered->buff, 3)
			);

			/* Redirect packet to the application data router */
			for(int i = 0; i < command_count; i++) {
				if(rx_packet->payloadType == command_list[i].packet_compatibility) {
					debug_leds_update(3);

					if(rx_packet->payloadType == PACKET_CMD) {
						/* Route this packet to the shell packet router function */
						shell_router(rx_packet);
					} else {
						command_list[i].command_function(-1, (char**)rx_packet);
					}
					break;
				}
			}

			/* Throw away used packet */
			discard_packet(rx_packet);

			/* Pop the packet bytes from the byte buffer */
			int j = 0;
			for(int i = packet_byte_buffered->buff_fifo_sz + 1; i < packet_byte_buffered->bufflen; i++, j++)
				packet_byte_buffered->buff[j] = packet_byte_buffered->buff[i];
			for(; j < packet_byte_buffered->bufflen; j++)
				packet_byte_buffered->buff[j] = 0;

			if(packet_byte_buffered->buff_fifo_sz >= total_packet_size)
				packet_byte_buffered->buff_fifo_sz -= total_packet_size;

			/* Reset reception buffer */
			reset_rx_buffer(packet_byte_buffered->buff_fifo_sz == 0);
		}
	}
}

packet_t * create_packet(void * payload_data, uint32_t payload_length, enum PacketType payload_type) {
	packet_t * new_packet   = (packet_t*)malloc(sizeof(packet_t));
	new_packet->magic0      = STARTPACKET_MAGIC0;
	new_packet->magic1      = STARTPACKET_MAGIC1;
	new_packet->payloadSize = payload_length;
	new_packet->payloadType = payload_type;
	new_packet->payload     = (char*)malloc(payload_length);
	memcpy(new_packet->payload, payload_data, payload_length);

	return new_packet;
}

void discard_packet(packet_t * packet) {
	if(is_packet((uint8_t*)packet)) {
		if(packet->payload)
			free(packet->payload);
		free(packet);
	}
}

void packetman_send_packet(void * payload_data, enum PacketType payload_type) {
	while(packetman_is_busy())
		rtos_preempt(); /* Spin the thread until the packet manager is available */

	packet_transmission_lock = true; /* Lock this function */

	int payload_size = packet_type_sizes[payload_type];

	/* Create packet with no payload */
	packet_t packet;
	packet.magic0      = STARTPACKET_MAGIC0;
	packet.magic1      = STARTPACKET_MAGIC1;
	packet.payloadSize = payload_size;
	packet.payloadType = payload_type;

	enum STREAM_MODE old_stream_mode = stream_mode;
	stream_mode = STREAM_PACKET;

	debug_leds_update(2);

	/* Send the header first in big endian format */
	for(int i = 0; i < PACKET_HEADER_SIZE; i += sizeof(int))
		for(int j = sizeof(int) - 1; j >= 0; j--)
			write(1, &((uint8_t*)&packet)[i + j], 1);

	/* And the payload second */
	write(1, payload_data, payload_size);

	stream_mode = old_stream_mode;

	packet_transmission_lock = false; /* And unlock it now */
}

extern piezo_t * module_piezo;

int packetman_connect_callback(int argc, char ** argv) {
	if(argc >= 0) return -1; /* Must be a packet */

	/* PC / Android app is requesting connection */

	packet_connect_t * rx_payload = (packet_connect_t*)((packet_t*)argv)->payload;

	if(rx_payload->connRequest) {
		/* Send acknowledgment */
		packet_connect_t payload;
		payload.connAck = true;
		packetman_send_packet(&payload, PACKET_CONNECT);

		is_connected        = true;
		play_connected_tune = true;
	}

	return 0;
}

int packetman_disconnect_callback(int argc, char ** argv) {
	if(argc >= 0) return -1; /* Must be a packet */

	/* PC / Android app is requesting disconnection */

	packet_disconnect_t * rx_payload = (packet_disconnect_t*)((packet_t*)argv)->payload;

	if(rx_payload->disconnRequest) {
		/* Send acknowledgment */
		packet_disconnect_t payload;
		payload.disconnAck = true;
		packetman_send_packet(&payload, PACKET_DISCONNECT);

		is_connected           = false;
		play_disconnected_tune = true;
	}

	return 0;
}

int packetman_keepalive_callback(int argc, char ** argv) {
	if(argc >= 0) return -1; /* Must be a packet */

	/* PC / Android app is acknowledging its presence */

	if(((packet_keepalive_t*)(packet_keepalive_t*)((packet_t*)argv)->payload)->iAmAlive)
		received_keepalive_packet = true;

	return 0;
}

void update_communication(void) {
	/* Handle reception timeouts for byte streams */
	if(!stream_byte_timeout && stream_mode != STREAM_PACKET && packet_byte_buffered != NULL) {
		if(update_byte_stream_delta > BYTESTREAM_TIMEOUT) {
			/* We timed out.
			 * We must listen for more packets (or regular bytes) */
			stream_byte_timeout = true;
			update_byte_stream_delta = 0;
			push_byte_stream();
		} else {
			update_byte_stream_delta++;
		}
	}

	switch(++update_delta) {
	case PERIOD_KEEPALIVE: {
		debug_leds_reset_all();

		/* Restart the update cycle */
		update_delta = 0;

		if(!is_connected)
			break;

		/* If we have not received a keep alive packet in the past cycles, then we lost connection */
		if(!received_keepalive_packet) {
			if(++keepalive_timeout_counter >= KEEPALIVE_MAXCOUNTER) {
				is_connected = false;

				/* Summon the disconnect callback by creating a dummy disconnect packet */
				packet_disconnect_t dummy_disconnect_payload;
				dummy_disconnect_payload.disconnRequest = true;
				packet_t * dummy_disconnect_packet = create_packet(&dummy_disconnect_payload, sizeof(packet_disconnect_t), PACKET_DISCONNECT);
				packetman_disconnect_callback(-1, (char**)dummy_disconnect_packet);
				discard_packet(dummy_disconnect_packet);

				keepalive_timeout_counter = 0;
			}
		} else {
			received_keepalive_packet = false;
			keepalive_timeout_counter = 0;
		}

		break;
	}
	case PERIOD_KEEPALIVE / 2: {
		if(!is_connected)
			break;

		packet_keepalive_t payload;
		payload.iAmAlive = true;
		packetman_send_packet(&payload, PACKET_KEEPALIVE);
		break;
	}
	}

	/* Play a tune when there's a connection and disconnection event */
	if(module_piezo && !module_piezo->is_playing) {
		if(play_connected_tune)
			play_connected_tune = piezo_play_song_async(module_piezo, tune_connected, arraysize(tune_connected), false) != PIEZO_SONG_DONE;
		else if(play_disconnected_tune)
			play_disconnected_tune = piezo_play_song_async_backwards(module_piezo, tune_connected, arraysize(tune_connected), false) != PIEZO_SONG_DONE;
	}
}

enum STREAM_MODE packetman_get_comms_status(void) {
	return stream_mode;
}

bool packetman_is_connected(void) {
	return is_connected;
}

bool packetman_is_busy(void) {
	return packet_transmission_lock;
}

void packetman_task(void * args) {
	while(true) {
		update_communication();

		/* Also update RTOS timeout service */
		rtos_update_timeout_service(PERIOD_UPDATE);
	}
}

void packetman_init() {
	is_packetman_init = true;

	/* Create and initialize HC-06 Bluetooth module first */
	bluetooth_init();

	/* Create a buffer for every byte received */
	packet_byte_buffered = fifo_create(PACKETMAN_RXBUFFSIZE);

	/* Reset RTOS timeout service */
	rtos_reset_timeout_service();

	rtos_spawn_task("packetman_task", packetman_task);
}
