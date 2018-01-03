#include "Database_lib\Database_handler\database_stud.h"

// Constant for the database filename's max buffer size
#define DATABASE_FILENAME_SIZE 50
#define SEARCH_KEYWORD_MAXSIZE 50

#pragma region GLOBAL VARIABLES
// MENU ENUMS:
// Menus available:
typedef enum { 
	MENU_START, MENU_MAIN, SUBMENU_START_OPEN, SUBMENU_START_NEW, SUBMENU_START_DEL, SUBMENU_MAIN_SEARCH
} Menu_t;

// Menu 'start' choices:
typedef enum { 
	MENU_START_OPEN = 'O', MENU_START_NEW = 'N', MENU_START_DEL = 'D', MENU_EXIT = 'E'
} Menu_Start_Options;

// Menu 'main' choices:
typedef enum {
	CREATE_STUD = '1', INSERT_NEW = '2', DISPLAY_WHOLE = '3', DISPLAY_SINGLE = '4',
	SEARCH_STUD = '5', DELETE_STUD = '6', SAVE_DATA = 'S', GO_BACK = 'B'
} Menu_Main_Options;

// Global variables:
Menu_t current_menu = MENU_START; // For tracking the menu the user is currently present on
char mode; // What mode is the database open on. Available choices: New / Open
char database_filename[DATABASE_FILENAME_SIZE]; // Filename of the database
bool initialized = false; // For tracking whether or not the database was initialized. Required for when loading a database a second time.
bool just_saved = false; // Required for user interface purposes
#pragma endregion

#pragma region FUNCTION PROTOTYPES
// Function prototypes:
// Menu prototypes:
void menu_start();
void menu_main();

// Submenu prototypes:
void submenu_start_open();
void submenu_start_new();
void submenu_start_delete_database();
void submenu_main_search();

// Function to use on the sub start menu to check whether or not the user wants to return to the start menu
bool go_back();
#pragma endregion

void main() {
	// Initialize console handle for changing console colors:
	consoleHandleInit(); 
	
	// Core of the program: Super loop with a switch which decides which menu we're in
	while (current_menu != MENU_EXIT) {
		switch (current_menu) {
			case MENU_START:
				menu_start();
				break;
			case MENU_MAIN:
				menu_main();
				break;
			case SUBMENU_START_OPEN:
				submenu_start_open(); 
				break;
			case SUBMENU_START_NEW:
				submenu_start_new(); 
				break;
			case SUBMENU_START_DEL:
				submenu_start_delete_database();
				break;
			default:
				current_menu = MENU_START;
				break;
		}
	}

	// Don't close the program with the database allocated and the file opened:
	if (initialized) 
		cleanup_database();

	// At this point, the program has ended:
	printf("\n\n\t\t\t    ");
	fcolor(LRED); bcolor(BWHITE);
	printf("- Press any key to exit.");
	getch();
}

#pragma region MENUS
void menu_main() {
	char main_choice;
	int insert_where;
	int database_display_limit_start, database_display_limit_end;
	int show_which_student;
	int delete_which_student;

	// Update current menu's variable:
	current_menu = MENU_MAIN;
	
	// Set color of the console:
	fcolor(BWHITE); bcolor(AQUA);
	clear_screen();
	fflush(stdin); // Flush the standard input because of the getch() function

	if (!initialized) {
		initialize_database(mode, database_filename);
		initialized = true;
	}

	if (just_saved) {
		bcolor(BWHITE); fcolor(AQUA);
		printf(">> Database '%s' saved!", database_filename);
		bcolor(AQUA); fcolor(BWHITE);
		just_saved = false;
	}
	
	printf("\n\n\n\n\n\n\t\t\t");
	bcolor(BWHITE); fcolor(AQUA);
	printf(">> MENU: Main (Database : '%s')", database_filename);
	bcolor(AQUA); fcolor(BWHITE);
	printf("\n\n\t\t\t1 - Create new student\n"\
		"\t\t\t2 - Insert new student\n"\
		"\t\t\t3 - Display database\n"\
		"\t\t\t4 - Display student\n"\
		"\t\t\t5 - Search for student(s)\n"\
		"\t\t\t6 - Delete student\n"\
		"\t\t\tS - Save database\n"\
		"\t\t\tB - Go back to start menu\n"\
		"\t\t\tE - Exit\n\n\t\t\t");
	bcolor(BWHITE); fcolor(AQUA);
	printf("> Choose:");
	bcolor(AQUA); fcolor(BWHITE);
	printf(" ");
	scanf("%c", &main_choice);
	main_choice = toupper(main_choice);

	switch (main_choice) {
		case CREATE_STUD:
			add_student(); // Create new student and add him to the top of the linked list
			break;
		case INSERT_NEW:
			while (1) {
				clear_screen();
				display_database(1, get_database_size());
				insert_where = input_int("- In which index do you want to insert the new student? (enter 0 to go back): ");
				if (insert_where == 0) 
					break; // Go back to main menu

				// Else: insert new student:
				if (insert_where > 0) {
					insert_student(insert_where);
					break;
				} else {
					printf("> The index is not valid! Press any key and try again.");
					getch();
				}
			}
			break;
		case DISPLAY_WHOLE:
			clear_screen();
			printf("- Do you want to see the entire database? It is recommended to limit the output. (Y/n): ");
			if (toupper(getch()) != 'Y') {
				database_display_limit_start = input_int("\n> Enter the first index to show (starting index, -1 for default): ");
				database_display_limit_end = input_int("> Enter the last index to show (ending index, -1 for default): ");
				if (database_display_limit_end == -1) 
					database_display_limit_end = get_database_size();
				display_database(database_display_limit_start, database_display_limit_end);
			}
			else display_database(1, get_database_size());

			printf("\n> Press any key to return to the main menu.");
			getch();
			break;
		case DISPLAY_SINGLE:
			clear_screen();
			show_which_student = input_int("- Enter the student's index on the database: ");
			display(show_which_student);

			printf("\n> Press any key to return to the main menu.");
			getch();
			break;
		case SEARCH_STUD:
			// If the database is empty then the user won't be able to search a student:
			if (get_database_size() == 0) {
				clear_screen();
				printf("- The database is empty. Cannot search student. Press any key to return.");
				getch();
			}else
				submenu_main_search();
			break;
		case DELETE_STUD:
			while (1) {
				clear_screen();
				// If the database is empty then the user won't be able to delete a student:
				if (get_database_size() == 0) {
					printf("- The database is empty. Cannot delete student. Press any key to return.");
					getch();
					break;
				}

				display_database(1, get_database_size());
				delete_which_student = input_int("- Enter the student's index in order to delete him (enter 0 to go back): ");
				if (delete_which_student == 0)
					break; // Go back to main menu

				// Else delete student:
				if (delete_which_student > 0) {
					delete_student(delete_which_student);
					break;
				} else {
					printf("> The index is not valid! Press any key and try again.");
					getch();
				}
			}
			break;
		case SAVE_DATA:
			save_data();
			just_saved = true;
			break;
		case GO_BACK:
			initialized = false;
			cleanup_database();
			current_menu = MENU_START;
			break;
		case MENU_EXIT:
			current_menu = MENU_EXIT;
			break;
	}
}

void menu_start() {
	char start_choice;

	// Set starting color of the console:
	fcolor(BWHITE);
	bcolor(LBLUE);
	clear_screen();

	// Print program intro with colors:
	printf("\n\n\t\t   ");
	bcolor(BLUE);	fcolor(BWHITE);	printf(" Module :");
	bcolor(BWHITE); fcolor(BLACK);	printf(" EC1S05 - Introduction to C Programming \n");
	
	bcolor(LBLUE);	fcolor(BWHITE);	printf("\t\t   ");
	bcolor(BLUE);   fcolor(BWHITE);	printf(" Program:"); 
	bcolor(BWHITE); fcolor(BLACK);	printf(" Student Database                       \n");
	
	bcolor(LBLUE);	fcolor(BWHITE); printf("\t\t   ");
	bcolor(BLUE);   fcolor(BWHITE); printf(" Course :");
	bcolor(BWHITE); fcolor(BLACK); printf(" BEng Computer Systems Eng. Year 1      \n");
	
	bcolor(LBLUE);	fcolor(BWHITE); printf("\t\t   ");
	bcolor(BLUE);   fcolor(BWHITE); printf(" Student:");
	bcolor(BWHITE); fcolor(BLACK); printf(" 14031329 Miguel Santos                 \n");
	
	fcolor(BWHITE);
	bcolor(LBLUE);

	// Format output by changing colors, applying tabs and newlines:
	printf("\n\n\t\t\t\t");
	bcolor(BWHITE); fcolor(LBLUE);
	printf(">> MENU: Start");
	bcolor(LBLUE); fcolor(BWHITE);
	printf("\n\n\t\t\t\tO - Open database\n"\
			"\t\t\t\tN - New database\n"\
			"\t\t\t\tD - Delete database\n"\
			"\t\t\t\tE - Exit\n\n\t\t\t\t");

	bcolor(BWHITE); fcolor(LBLUE);
	printf("> Choose:");
	bcolor(LBLUE); fcolor(BWHITE);
	printf(" ");
	// Ask for user input:
	start_choice = toupper(getch());

	// Decide to which menu to go:
	switch (start_choice) {
		case MENU_START_OPEN:
			current_menu = SUBMENU_START_OPEN;
			break;
		case MENU_START_NEW:
			current_menu = SUBMENU_START_NEW;
			break;
		case MENU_START_DEL:
			current_menu = SUBMENU_START_DEL;
			break;
		case MENU_EXIT: current_menu = MENU_EXIT;
			break;
	}
}
#pragma endregion

#pragma region SUB MENUS
void submenu_start_open(){
	char open_choice;

	clear_screen();
	
	printf("\n\n\n\n\n\n\n\t\t\t\t");
	bcolor(BWHITE); fcolor(LBLUE);
	printf(">> MENU: Open database");
	bcolor(LBLUE); fcolor(BWHITE);
	printf("\n\n- Enter the database file name (including extension, type 'B' to return)\n\n\t\t\t\t");
	
	bcolor(BWHITE); fcolor(LBLUE);
	printf("Enter: ");
	scanf("%"MACRO_TOSTR(DATABASE_FILENAME_SIZE)"s", database_filename);
	bcolor(LBLUE); fcolor(BWHITE);

	// Check first if the user wanted to come back:
	if (go_back()) return;

	// Then check if the file exists
	if (file_exists(database_filename)){
		// If it does, set the database's operation to 'open mode'
		mode = MODE_OPEN;
		// And go to menu main:
		menu_main();
	} // Else the file doesn't exist and ask the user to go back or try again:
	else{
		printf("\n>> The file '%s' does not exist! Press any key to try again, 'E' to exit or 'B' to go back. ", database_filename);
		open_choice = toupper(getch());
		if (open_choice == MENU_EXIT)
			current_menu = MENU_EXIT;
		else if (open_choice == GO_BACK)
			current_menu = MENU_START;
	}
}

void submenu_start_new(){
	bool fileAlreadyThere = true;

	while (fileAlreadyThere) {
		clear_screen();
		
		printf("\n\n\n\n\n\n\n\t\t\t\t");
		bcolor(BWHITE); fcolor(LBLUE);
		printf(">> MENU: New database");
		bcolor(LBLUE); fcolor(BWHITE);
		printf("\n\n- Enter the new database file name (including extension, type 'B' to return)\n\n\t\t\t\t");

		bcolor(BWHITE); fcolor(LBLUE);
		printf("Enter: ");
		scanf("%"MACRO_TOSTR(DATABASE_FILENAME_SIZE)"s", database_filename);
		bcolor(LBLUE); fcolor(BWHITE);

		// Check first if the user wanted to come back:
		if (go_back()) return;

		/*	Check if the file already exists.
		*	And if so, ask if the user wants to overwrite it.	
		*/
		if (file_exists(database_filename)) {
			printf("- The file '%s' already exists. Are you sure you want to overwrite it? (N/y): ", database_filename);
			if (toupper(getch()) == 'Y') 
				fileAlreadyThere = false;
		} // The file is brand new. No need for overwriting:
		else fileAlreadyThere = false;
	}

	// Set mode of the database's operation:
	mode = MODE_NEW;
	menu_main();
}

void submenu_start_delete_database(){
	while (1) {
		clear_screen();
		
		printf("\n\n\n\n\n\n\n\t\t\t\t");
		bcolor(BWHITE); fcolor(LBLUE);
		printf(">> MENU: Delete database");
		bcolor(LBLUE); fcolor(BWHITE);
		printf("\n\n- Enter the database filename to delete (including ext, type 'B' to return)\n\n\t\t\t\t");
		
		bcolor(BWHITE); fcolor(LBLUE);
		printf("Enter: ");
		scanf("%"MACRO_TOSTR(DATABASE_FILENAME_SIZE)"s", database_filename);
		bcolor(LBLUE); fcolor(BWHITE);
		
		// Check first if the user wanted to come back:
		if (go_back()) return;
		
		if (!file_exists(database_filename)) {
			printf("> File '%s' does not exist! Press any key to try again.", database_filename);
			getch();
		}
		else {
			printf("> Are you sure? (N/y): ");
			if (toupper(getch()) == 'Y') {
				database_file = database_filename;
				delete_file();
				printf("\n> Database '%s' was successfully deleted from the system.\n  Press any key to continue.", database_filename);
				getch();
				break;
			}
		}
	}
	current_menu = MENU_START;
}

void submenu_main_search() {
	int mode_choice;
	int filter_choice;
	int filter_carrier = 0;
	char keyword[SEARCH_KEYWORD_MAXSIZE];
	char EN[9] = "Enabled";
	char DIS[9] = "Disabled";

	// First set up all search modes and filters by asking the user:
	while (1) { // Only get out of the search menu when the user entered a valid option
		clear_screen();
		
		printf("\n\n\n\n\n\n\t\t\t"); 
		bcolor(BWHITE); fcolor(AQUA);
		printf(">> MENU: Search");
		bcolor(AQUA); fcolor(BWHITE);
		printf("\n\n"\
			"\t\t\t> What do you want to search by?\n\n"\
			"\t\t\t1 - Student's ID\n"\
			"\t\t\t2 - Student's first name\n"\
			"\t\t\t3 - Student's last name\n"\
			"\t\t\t4 - Student's birth date (use format: xx/xx/xxxx)\n"\
			"\t\t\t5 - Students birth day\n"\
			"\t\t\t6 - Student's birth month\n"\
			"\t\t\t7 - Student's birth year\n"\
			"\t\t\t8 - Go back\n\n\t\t\t");
		bcolor(BWHITE); fcolor(AQUA);
		printf("> Choose:");
		bcolor(AQUA); fcolor(BWHITE);
		printf(" ");
		mode_choice = input_int("");
		
		if (mode_choice == 8) // Returning to the main menu
			return;
		else if (mode_choice < 8 && mode_choice > 0) // Valid option. Getting out of the search loop
			break;
	}

	// The next line of code: this assignment is because of the way search filters are used
	// Check header 'database_search.h' for more information
	filter_carrier = mode_choice - 1;
	printf("\t\t\t");
	bcolor(BWHITE); fcolor(AQUA);
	printf("> Enter your keyword:");
	bcolor(AQUA); fcolor(BWHITE);
	printf(" ");
	scanf("%"MACRO_TOSTR(SEARCH_KEYWORD_MAXSIZE)"s", keyword);

	clear_screen();
	
	printf("\n\n\t\t\t>>Setting filters\n\n\t\t\t");
	bcolor(BWHITE); fcolor(AQUA);
	printf("> Do you wish to apply");
	bcolor(AQUA); fcolor(BWHITE);
	printf("\n\t\t\t");
	bcolor(BWHITE); fcolor(AQUA);
	printf("filters to your search ? (Y/n):");
	bcolor(AQUA); fcolor(BWHITE);
	printf(" ");
	if (toupper(getch()) == 'Y') {
		// Ask for filters:
		while (1) {
			clear_screen();
			
			// Format the display: (it may be complicated to read unfortunately)
			printf("\n\n\t\t\t>>Setting filters\n\n\t\t\t1 - Enable case sensitive (");  
			if (filter_carrier & SEARCH_FILTER_CASE) fcolor(LGREEN), printf(EN);
			else fcolor(RED), printf(DIS); fcolor(BWHITE);
			
			printf(")\n\t\t\t2 - Match first letter only ("); 
			if (filter_carrier & SEARCH_FILTER_FIRSTLETTER) fcolor(LGREEN), printf(EN);
			else fcolor(RED), printf(DIS); fcolor(BWHITE);
			
			printf(")\n\t\t\t3 - Match last letter only (");
			if (filter_carrier & SEARCH_FILTER_LASTLETTER) fcolor(LGREEN), printf(EN);
			else fcolor(RED), printf(DIS); fcolor(BWHITE);

			printf(")\n\t\t\t4 - Match bigger words only (");
			if (filter_carrier & SEARCH_FILTER_GREATER) fcolor(LGREEN), printf(EN);
			else fcolor(RED), printf(DIS); fcolor(BWHITE);

			printf(")\n\t\t\t5 - Match smaller words only (");
			if (filter_carrier & SEARCH_FILTER_LESS) fcolor(LGREEN), printf(EN);
			else fcolor(RED), printf(DIS); fcolor(BWHITE);

			printf(")\n\t\t\t6 - Invert matching (");
			if (filter_carrier & SEARCH_FILTER_INV) fcolor(LGREEN), printf(EN);
			else fcolor(RED), printf(DIS); fcolor(BWHITE);

			printf(")\n\t\t\t7 - Remove the day from birth date (");
			if (filter_carrier & SEARCH_FILTER_DEL_DAY) fcolor(LGREEN), printf(EN);
			else fcolor(RED), printf(DIS); fcolor(BWHITE);

			printf(")\n\t\t\t8 - Remove the month from birth date (");
			if (filter_carrier & SEARCH_FILTER_DEL_MONTH) fcolor(LGREEN), printf(EN);
			else fcolor(RED), printf(DIS); fcolor(BWHITE);

			printf(")\n\t\t\t9 - Remove the year from birth date (");
			if (filter_carrier & SEARCH_FILTER_DEL_YEAR) fcolor(LGREEN), printf(EN);
			else fcolor(RED), printf(DIS); fcolor(BWHITE);

			printf(")\n\n\t\t\t10 - ");
			fcolor(LGREEN);	printf("!Search the database!\n"); fcolor(BWHITE);
			printf("\t\t\t0 - Go back\n\n\t\t\t");
			bcolor(BWHITE); fcolor(AQUA); printf("> Choose:");	bcolor(AQUA); fcolor(BWHITE);
			printf(" ");
			filter_choice = input_int("");
			
			if (filter_choice == 0) return;
			else if (filter_choice == 10) break;

			// Else -> toggle filters:
			if (filter_choice>0 && filter_choice<10)
				filter_carrier ^= (1 << filter_choice + 2);
		}
	}

	// And finally search and display the results
	display_list(search_database(filter_carrier, keyword), keyword);

	printf("\n> Press any key to return to the main menu.");
	getch();
}
#pragma endregion

bool go_back() {
	// Using this function on the "start menu"'s submenu: Open database, New database and Delete database
	bool going_back = false;
	// Check first if the user wanted to come back:
	if (strcmp(database_filename, "B") == 0 || strcmp(database_filename, "b") == 0) { // Compare with "B". If it was indeed "B" then the user wanted to go back to start menu
		current_menu = MENU_START;
		going_back = true;
	}
	return going_back;
}