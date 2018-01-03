#include "database_stud.h"

void initialize_database(char mode, char * database_path){
	int i; 
	/* This array will hold the parsed records fetched from the database's file
	 * and will be pushed into the linked list after it is loaded
	 */
	Database_Student ** loaded_database = NULL; 
		
	initialize_linked_list();
	
	// If mode is set to create a new database:
	if (mode == MODE_NEW)
		create_file(database_path); // Create new file using 'database_path':
	else if (mode == MODE_OPEN){ // Else open a given database
		
		// Load file using database_path
		load_database(database_path);
		// and parse: (populate linked list with this array)
		loaded_database = parse_database(database_content, file_linecount);
		
		// Finally populate the linked list with the loaded and parsed text from the database file:
		for (i = 0; i < file_linecount; i++)
			push_node(loaded_database[i]);
	}

	free(loaded_database);
}

void save_data(){
	// First fetch all nodes from the linked list:
	Database_Student ** whole_data = fetch_all_nodes();
	// Save this fetched data into the file:
	save_database(whole_data, allocated_nodes);
	// And free the data acquired from memory:
	free(whole_data);
}

void display_database(int start, int limit_output){
	int database_size = get_database_size();
	int i;
	Database_Student * current_student;

	bcolor(GREEN); fcolor(BWHITE);
	clear_screen();
	
	// If the user entered invalid limits, put a default number on them:
	if (start < 1)
		start = 1;
	else if (start>allocated_nodes)
		start = allocated_nodes;

	if (limit_output < 1)
		limit_output = 1;
	else if (limit_output>allocated_nodes)
		limit_output = allocated_nodes;

	// Cap the limits' values:
	if (start > limit_output)
		start = limit_output;
	if (limit_output < start)
		limit_output = start;
	
	// Display database:
	if (allocated_nodes == 0) {
		printf("\n>> Displaying database (0 out of 0 records)\n\n");
		bcolor(BWHITE); fcolor(RED);
		printf("!! There are no records on the database !!\n");
		bcolor(GREEN); fcolor(BWHITE);
	}
	else{
		printf("\n>> Displaying database (%d out of %d records)\n\n", limit_output - start + 1, allocated_nodes);
		for (i = start + 1; i <= allocated_nodes + 1 && i <= limit_output + 1; i++){
			current_student = get_node(i);

			printf("  > Student #%d - ID: %s | Name: %s %s | Birth date: %s/%s/%s;\n", i - 1,
				current_student->id,
				current_student->firstName,
				current_student->lastName,
				current_student->birthDate.day,
				current_student->birthDate.month,
				current_student->birthDate.year);
		}
	}
	printf("________________________________\n");
}

void display_list(int * indices, char * search_made){
	int indices_count = 0;
	int i;
	Database_Student * current_student;
	
	bcolor(GREEN); fcolor(BWHITE);
	clear_screen();

	// Find indices' array size:
	if (indices != NULL) {
		for (i = 0; indices[i] != NULL; i++);
		indices_count = i;
	}

	// And display data:
	if (indices_count == 1)
		printf("Displaying student #%d (Search: '%s'):\n\n", indices[0], search_made);
	else
		printf("\n>> Displaying list of students: (Search: '%s'. %d matches out of %d records)\n\n", search_made, indices_count, allocated_nodes);

	if (indices_count == 0) {
		bcolor(BWHITE); fcolor(RED);
		printf("> No matches found!\n");
		bcolor(GREEN); fcolor(BWHITE);
	}
	else {
		for (i = 0; i < indices_count; i++) {
			current_student = get_node(indices[i] + 1); // +1 to avoid the null student

			printf("  > Student #%d - ID: %s | Name: %s %s | Birth date: %s/%s/%s;\n", indices[i],
				current_student->id,
				current_student->firstName,
				current_student->lastName,
				current_student->birthDate.day,
				current_student->birthDate.month,
				current_student->birthDate.year);
		}
	}
	printf("________________________________\n");
	free(indices);
}

void display(int index){
	display_database(index, index);
}

int * search_database(int search_mode, char * keyword){
	return database_query(search_mode, keyword);
}

Database_Student * form_create_student(char * task_name){
	Database_Student * new_student;
	char * tmp_id, * tmp_firstName, * tmp_lastName;
	int tmp_day, tmp_month, tmp_year;

	clear_screen();

	// Ask for data and allocate student, then return it
	printf("\n> %s (%d existing)\n\n", task_name, allocated_nodes);
	
	tmp_id = input_string("  - Enter student ID: ");
	tmp_firstName = input_string("  - Enter first name: ");
	tmp_lastName = input_string("  - Enter last name: ");
	
	tmp_day = input_int("\n  > Birthdate:\n     - Enter day (0-31): ");
	tmp_month = input_int("     - Enter month (0-12): ");
	tmp_year = input_int("     - Enter year: ");
	
	// Allocate new student using entered data:
	new_student = allocate_database_node(tmp_id, tmp_firstName, tmp_lastName, tmp_day, tmp_month, tmp_year);
	
	free(tmp_id);
	free(tmp_firstName);
	free(tmp_lastName);

	// And return newly allocated student:
	return new_student;
}

void add_student(){
	/* Create a new student using a form
	 * And push him into the linked list
	 */
	push_node(form_create_student(TASKNAME_ADDING));
}

void insert_student(int index){
	/* Allocate student using a form 
	 * and insert it on the linked list
	 */
	insert_node(form_create_student(TASKNAME_INSERTING),index);
}

void delete_student(int index){
	// Deallocate student from linked list:
	delete_node(index);
}

int get_database_size(){
	return allocated_nodes;
}

void empty_database(){
	int i;
	int nodes_available = allocated_nodes;
	
	// The database is not 0 based. The records start at 1:
	for (i = 1; i <= nodes_available; i++)
		delete_node(i);
}

void delete_database(){
	delete_file();
}

void cleanup_database(){
	// free the linked list and the module 'file's variables:
	cleanup_linked_list();
	cleanup_file();
}