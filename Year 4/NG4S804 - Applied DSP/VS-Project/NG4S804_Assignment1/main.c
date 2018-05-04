#include <windows.h>
#include <conio.h>
#include <stdbool.h>
#include <stdio.h>

/* Function prototypes for the functions
   being developed on the assignment */
void question1_a(int dft_points, bool use_hann_window, bool use_double_sided_spectrum);
void question1_b(int dft_points, bool use_hann_window, bool use_double_sided_spectrum);
void question1_c(int dft_points, bool use_hann_window, bool use_double_sided_spectrum);

/* More function prototypes */
void goto_XY(int column, int line);
void print_menu(int cursor_position);
void run_question(int question, bool run_as_dsb);
int  get_adjusted_cursor_xcoord(int cursor_y_position);

#define QUESTION_COUNT  10
#define CURSOR_Y_OFFSET 8

int main(int argc, char ** argv)
{
	int cursor_position = 0;
	bool quit = false;

	print_menu(0);

	while(!quit) {
		bool arrow_pressed = true;
		char key = _getch();

		if (key == -32) {
			switch (_getch()) {
				case 72: /* Up arrow */
					if(cursor_position > 0)
						cursor_position--;
					break;
				case 80: /* Down arrow */
					if(cursor_position < QUESTION_COUNT)
						cursor_position++;
					break;
				case 75: /* Left arrow */
					if(cursor_position == QUESTION_COUNT)
						quit = true;
					else
						run_question(cursor_position, false);
					break;
				case 77: /* Right arrow */
					if (cursor_position == QUESTION_COUNT)
						quit = true;
					else
						run_question(cursor_position, true);
					break;
				default: arrow_pressed = false; break;
			}

			if(arrow_pressed) {
				printf("\b\b  ");
				goto_XY(get_adjusted_cursor_xcoord(cursor_position), cursor_position + CURSOR_Y_OFFSET);
				printf("<<");
			}
		} else if (key == 13) {
			/* Pressed ENTER */
			if (cursor_position == QUESTION_COUNT)
				quit = true;
			else
				run_question(cursor_position, false);
		} else if (key == 'q' || key == 'Q') {
			quit = true;
		}
	}

	return 0;
}

void goto_XY(int column, int line)
{
	COORD coord;
	coord.X = column;
	coord.Y = line;
	HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(consoleHandle, &info);
	SetConsoleCursorPosition(consoleHandle, coord);
}

void print_menu(int cursor_position)
{
	printf("***********************************************");
	printf("\n  NG4S804 - Applied Digital Signal Processing");
	printf("\n***********************************************");
	printf("\n\t      (Assignment 1)\n        (14031329 - Miguel Santos)\n");
	printf("\n>> Select the question\n");
	printf("\n> Q1.A \t (256 DFT SSB)");
	printf("\n> Q1.B \t (256 DFT SSB)");
	printf("\n> Q1.C \t (256 DFT SSB)");
	printf("\n> Q2.1.A (512 DFT SSB)");
	printf("\n> Q2.1.B (512 DFT SSB)");
	printf("\n> Q2.1.C (512 DFT SSB)");
	printf("\n> Q2.2.A (512 DFT SSB + Hann)");
	printf("\n> Q2.2.B (512 DFT SSB + Hann)");
	printf("\n> Q2.2.C (512 DFT SSB + Hann)");
	printf("\n> Dump all graphs");
	printf("\n>> Quit");
	printf("\n\n(move arrow UP/DOWN to select, LEFT/ENTER to run question OR press RIGHT to run question as DSB (Double Sided Spectrum))\n");

	goto_XY(get_adjusted_cursor_xcoord(cursor_position), cursor_position + CURSOR_Y_OFFSET);
	printf("<<");
}

int get_adjusted_cursor_xcoord(int cursor_y_position)
{
	if     (cursor_y_position < QUESTION_COUNT - 4) return 23;
	else if(cursor_y_position < QUESTION_COUNT - 1) return 30; 
	else if(cursor_y_position < QUESTION_COUNT)     return 18;
	else                                            return 8;
}

void run_question(int question, bool run_as_dsb)
{
	goto_XY(0, CURSOR_Y_OFFSET + QUESTION_COUNT + 5);
	switch (question) {
	case 0: printf(">> Executing question 1.a...\n");   question1_a(256, false, run_as_dsb); break;
	case 1: printf(">> Executing question 1.b...\n");   question1_b(256, false, run_as_dsb); break;
	case 2: printf(">> Executing question 1.c...\n");   question1_c(256, false, run_as_dsb); break;
	case 3: printf(">> Executing question 2.1.a...\n"); question1_a(512, false, run_as_dsb); break;
	case 4: printf(">> Executing question 2.1.b...\n"); question1_b(512, false, run_as_dsb); break;
	case 5: printf(">> Executing question 2.1.c...\n"); question1_c(512, false, run_as_dsb); break;
	case 6: printf(">> Executing question 2.2.a...\n"); question1_a(512, true,  run_as_dsb); break;
	case 7: printf(">> Executing question 2.2.b...\n"); question1_b(512, true,  run_as_dsb); break;
	case 8: printf(">> Executing question 2.2.c...\n"); question1_c(512, true,  run_as_dsb); break;
	case 9:
		printf(">> Executing all questions...\n");
		for (char i = 0; i <= 1; i++) {
			question1_a(256, false, (bool)i);
			question1_b(256, false, (bool)i);
			question1_c(256, false, (bool)i);
			question1_a(512, false, (bool)i);
			question1_b(512, false, (bool)i);
			question1_c(512, false, (bool)i);
			question1_a(512, true,  (bool)i);
			question1_b(512, true,  (bool)i);
			question1_c(512, true,  (bool)i);
		}
		break;
	default: break;
	}
	printf("\n>> Finished executing. Press any key to go back up to the menu");
	_getch();
	system("cls");
	print_menu(question);
}
