#ifndef MISC_H_
#define MISC_H_

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <ctype.h>
#include <Windows.h>
#include "stdbool.h"

#define INPUT_MAXIMUMBUFFER	255
#define MACRO_TOSTR(N) MACRO_TOSTR2(N)
#define MACRO_TOSTR2(N) #N

// Necessary for changing the console's colors:
HANDLE consoleHandle;

/* Required for actually setting the colors
 * by using bit handling:
 */
unsigned char current_color;

typedef enum {
	BLACK, BLUE, GREEN, AQUA, RED, PURPLE, YELLOW, WHITE, GRAY, LBLUE,
	LGREEN, LAQUA, LRED, LPURPLE, LYELLOW, BWHITE
} Console_Colors_t;

// Misc functions:
extern char * input_string(char * message);
extern int input_int(char * message);
extern void clear_screen();
extern char * substring(char * src, int from, int length);
extern bool arr_contains(int * src, int size, int token);
extern void str_toupper(char * src);
extern void consoleHandleInit();
extern void bcolor(char color);
extern void fcolor(char color);

#endif