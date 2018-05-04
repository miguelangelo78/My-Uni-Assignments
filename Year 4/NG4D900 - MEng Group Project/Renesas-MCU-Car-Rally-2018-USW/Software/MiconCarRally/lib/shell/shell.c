/*
 * shell.c
 *
 *  Created on: 01/11/2017
 *      Author: Miguel
 */

#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "shell.h"
#include "shell_commands.h"

#pragma section BTLDR
#pragma section C CBTLDR

bool is_shell_init = false;
bool reset_shell   = false;
int  command_count = 0;

void shell_router(packet_t * packet) {
	/* TODO */
}

static void unrecognized_command(char * user_input) {
	printf("\nERROR: '%s' is not a recognized command\n", user_input);
	help(0, NULL);
}

static void prompt(void) {
	printf("\ncar> ");
}

static void banner(void) {
	console_clear(0, NULL);
	printf(" _       __     __                        \n\
| |     / /__  / /________  ____ ___  ___ \n\
| | /| / / _ \\/ / ___/ __ \\/ __ `__ \\/ _ \\\n\
| |/ |/ /  __/ / /__/ /_/ / / / / / /  __/\n\
|__/|__/\\___/_/\\___/\\____/_/ /_/ /_/\\___/ \n\
                                          \n");
}

void shell_init(void) {
	if(is_shell_init)
		return;

	/* Find out how many commands exist on the command_list array */
	for(int i = 0; i < SHELL_MAX_COMMAND_COUNT && command_list[i].command != NULL && command_list[i].command_function != NULL; i++)
	{
		command_count++;

		for(int j = 0; j < strlen(command_list[i].command); j++)
			command_list[i].command[j] = (char)tolower(command_list[i].command[j]);
	}

	is_shell_init = true;
}

void shell_task(void * args) {

	while(1) {
		/* Initialize the shell */
		shell_init();

		/* Show initialization message to the user */
		banner();

		if((uint8_t)((dipswitch_read() & 0x8) >> 3))
			puts("(ALERT: TEMPLATE MODE IS ENABLED)");

		/* Tell the user what command he can type to show all of the commands the car supports */
		puts("Type 'help' to display the supported commands.");

		while(!reset_shell) {
			/* Print the command prompt */
			prompt();

			/* Read user input */
			static char option[32];
			scanf("%32[^\r]", option);
			getchar();

			/* Find out the length of the command the user just inputted */
			size_t option_length = strlen(option);
			for(int i = 0; i < option_length; i++) {
				if(option[i] == ' ') {
					option_length = i;
					break;
				}

				/* Convert substring (the command string only) into lower case while we're at it */
				option[i] = (char)tolower(option[i]);
			}

			/* Count how many space characters this string has */
			int  spaces_found = 0;
			bool spaces_found_increment_next_cycle = false;

			for(int i = 0; i < strlen(option); i++) {
				if(option[i] == ' ') {
					spaces_found_increment_next_cycle = true;
				} else {
					if(spaces_found_increment_next_cycle) {
						spaces_found_increment_next_cycle = false;
						spaces_found++;
					}
				}
			}

			/* Find the command. If it exists, then execute it */
			bool command_exists = false;

			for(int i = 0; i < command_count; i++) {
				if(!strncmp(option, command_list[i].command, option_length)) {
					command_exists = true;

					/* We're incrementing because the command string is also an argument in itself */
					spaces_found++;

					/* Prepare arguments before calling the command */
					char ** argv = (char**)malloc(sizeof(char*) * spaces_found);

					if(spaces_found > 1) {
						/* Split option into separate arguments */
						char * token = strtok(option, " ");
						int ctr = 0;
						while(token != NULL && ctr < spaces_found) {
							argv[ctr] = (char*)malloc(strlen(token));
							strcpy(argv[ctr++], token);
							token = strtok(NULL, " ");
						}
					} else {
						*argv = (char*)malloc(strlen(option));
						strcpy(*argv, option);
					}

					/* And finally run the command */
					int retcode;
					if((retcode = command_list[i].command_function(spaces_found, argv)))
						printf("\n\n(SHELL): ret code %d\n", retcode);

					/* Free arguments */
					for(int j = 0; j < spaces_found; j++)
						free(argv[j]);
					free(argv);
					break;
				}
			}

			if(!command_exists)
				unrecognized_command(option);
		}

		reset_shell = false;
	}
}

void shell_reset(void) {
	reset_shell = true;
	fflush(stdin);
	console_clear(0, NULL);
}
