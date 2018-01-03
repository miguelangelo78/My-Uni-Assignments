#include "misc.h"

char* input_string(char * message){
	char input_localbuffer[INPUT_MAXIMUMBUFFER];
	char * input_result;

	// Flush standard input to prevent trailing characters, such as newlines:
	fflush(stdin);
	
	while (1) {
		printf(message);
		
		/* Limit the input by 'INPUT_MAXIMUMBUFFER'. Use a macro the convert the constant to string
		 * so the scanf can limit the input's size:
		 */
		if (!scanf("%"MACRO_TOSTR(INPUT_MAXIMUMBUFFER)"s", input_localbuffer)) {
			fcolor(RED);
			bcolor(BWHITE);
			printf(">> Invalid input! You must enter a valid string. Try again.\n");
			fflush(stdin);
			bcolor(AQUA);
			fcolor(BWHITE);
		}
		else break;
	}

	input_result = (char*) malloc(sizeof(char)*strlen(input_localbuffer) + 1);
	strcpy(input_result, input_localbuffer);

	return input_result;
}

int input_int(char * message){
	int input;
	
	// Flush standard input to prevent trailing characters, such as newlines:
	fflush(stdin);

	while (1) {
		printf(message);

		// Limit the input integer to 5 digits by using the format %5d.
		if (!scanf("%5d", &input)) {
			fcolor(RED);
			bcolor(BWHITE);
			printf(">> Invalid input! You must enter a valid 16-bit signed number. Try again.\n");
			fflush(stdin);
			bcolor(AQUA);
			fcolor(BWHITE);
		}
		else break;
	}
	return input;
}

void clear_screen(){
	// Clear screen using command line's command 'CLS'
	system("cls");
}

char * substring(char * src, int from, int length){
	int str_length = length - from + 1;
	char * new_substring = (char*) malloc(sizeof(char)*str_length);
	
	strncpy(new_substring, src + from, length - from);
	new_substring[str_length-1] = '\0';
	
	return new_substring;
}

bool arr_contains(int * src, int size, int token){
	int i;

	for (i = 0; i < size; i++)
		if (src[i] == token) 
			return true;

	return false;
}

void str_toupper(char * src){
	int i = 0;

	// Set 'src' to uppercase letters:
	while (src[i] != NULL){
		src[i] = toupper(src[i]);
		i++;
	}
}

void consoleHandleInit() {
	// This function resides in the library 'Windows.h':
	consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void bcolor(char color) {
	// Set console background color:
	current_color &= 0x0F; // Clear top nibble
	current_color |= (color & 0x0F) << 4; // And set new background color
	SetConsoleTextAttribute(consoleHandle, current_color);
}

void fcolor(char color) {
	// Set console front color:
	current_color &= 0xF0; // Clear bottom nibble
	current_color |= color & 0x0F; // And set new front color
	SetConsoleTextAttribute(consoleHandle, current_color);
}