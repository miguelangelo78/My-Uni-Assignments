#ifndef DEBUG_H_
#define DEBUG_H_

enum BoardLEDs {
	GREEN 	= GPIO_Pin_12,
	ORANGE 	= GPIO_Pin_13,
	RED 	= GPIO_Pin_14,
	BLUE 	= GPIO_Pin_15
};

#define DEBUGMODE 1
#define SET_BOARD_LED(led) if(DEBUGMODE) PDB(led, 1);

#define ERROR_KILL 0 /* Whenever an error occurs, the program halts immediately. This enables it. */

#define IDLE() { \
	/* Green LED: */ \
	SET_BOARD_LED(GREEN); \
	printf("program ended\n\n"); for(;;); }

#define ERROR(errmsg) { \
	/* Red LED: */ \
	SET_BOARD_LED(RED); \
	printf("ERROR: %s\n\nprogram ended\n\n", errmsg); \
	if(ERROR_KILL) for(;;); }

#define init_board_debug() init_gpio(GPIOD, 3, GREEN | ORANGE | RED | BLUE , 0);

#endif
