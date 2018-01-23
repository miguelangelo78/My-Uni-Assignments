/*
 * shell.h
 *
 *  Created on: 01/11/2017
 *      Author: Miguel
 */

#ifndef LIB_SHELL_SHELL_H_
#define LIB_SHELL_SHELL_H_

#include <communications/protocols/packetman/packetman.h>
#include <communications/protocols/packetman/packet_types.h>

#define SHELL_MAX_COMMAND_COUNT 100
#define SHELL_NULL_COMMAND {NULL, NULL, (enum PacketType)NULL}

typedef int (*cmd_function_t)(int, char **);

typedef struct {
	char *          command;
	cmd_function_t  command_function;
	enum PacketType packet_compatibility;
} cmd_t;

void shell_task(void * args);
void shell_reset(void);
void shell_router(packet_t * packet);

#endif /* LIB_SHELL_SHELL_H_ */
