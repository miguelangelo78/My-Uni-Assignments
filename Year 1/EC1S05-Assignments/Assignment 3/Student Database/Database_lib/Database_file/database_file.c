#include "database_file.h"

// Tracks if the database's file is loaded or not
bool loaded = false;

bool load_database(char * database_filepath){
	bool success = false;
	int i = 0;
	char * tmp_databuffer;
	char * tmp_currline;
	
	// Set database's file path:
	database_file = (char*) malloc(sizeof(char)*strlen(database_filepath)+1);
	strcpy(database_file, database_filepath); // Copy 'database_filepath' into 'database_file'
	get_file_line_count(); // Set variable 'file_linecount'
	get_file_byte_count(); // Set variable 'file_charcount'
	tmp_databuffer = read_file(); // Read whole text from file in order to parse it into a 2D array (each row is a line from the file)
	
	// Fill 'database_content':
	database_content = (char **) malloc(sizeof(char*)*(file_linecount + 1));
	
	// Split tmp_databuffer by '\n's and put the results inside database_content[i]
	tmp_currline = strtok(tmp_databuffer, NEWLINE); // this function is ignoring the extra \n's on start and end
	while (tmp_currline != NULL){
		database_content[i] = (char*) malloc(sizeof(char)*strlen(tmp_currline)+1);
		strcpy(database_content[i], tmp_currline);
		tmp_currline = strtok(NULL, NEWLINE);
		i++;
	}

	success = loaded = true;

	return success;
}

void save_database(Database_Student ** database_data, int entry_count){
	// For this function: read whole linked list
	// and write into file from scratch all properties from all records
	int i;
	char line_buffer[INPUT_MAXIMUMBUFFER];

	// In case the database is empty:
	if (entry_count == 0)
		entry_count = 1;
	
	// Clear the file first
	delete_file();
	// Then write to a new file:
	for (i = 0; i < entry_count; i++){
		// Format the text first. The text's format needs to match the parsing function or else it won't be able
		// to load the file and parse it into the linked list
		sprintf(line_buffer, "ID: %s | First: %s | LastName: %s | Birth: %s/%s/%s;\n",
			database_data[i]->id,
			database_data[i]->firstName,
			database_data[i]->lastName,
			database_data[i]->birthDate.day,
			database_data[i]->birthDate.month,
			database_data[i]->birthDate.year);
		write_file(line_buffer);
	}
	close_file();
}

// Necessary for checking if the file exists before opening it:
bool file_exists(char * filename){
	bool isThere = false;
	FILE * f_checkfile = fopen(filename, FILEMODE_READ);
	
	if (f_checkfile != NULL){
		// It's there. Make the bool true and close the file:
		isThere = true;
		fclose(f_checkfile);
	}

	return isThere;
}

void check_validfile(char * validmode){
	// This functions reopens the file with the proper mode
	if (!isOpen)
		open_file(validmode);
	else if (strcmp(file_currentmode, validmode) != 0){
		// Reopen in the proper mode
		close_file();
		open_file(validmode);
	}
}

void open_file(char * mode){
	if (isOpen) 
		return;
	
	// Open file and check if it opened successfully:
	if ((fp = fopen(database_file, mode)) == NULL) {
		printf("\n\nERROR: Couldn't open file %s. Press any key to exit . . .", database_file);
		getch();
		exit(-1);
	}

	// Update current mode in which the file is opened in:
	strcpy(file_currentmode, mode);
	isOpen = true;
}

void close_file(){
	if (!isOpen) 
		return;

	// Close the file:
	fclose(fp);
	// Update the mode to NULL mode, since the file is now closed:
	strcpy(file_currentmode, FILEMODE_NULL);
	isOpen = false;
}

char * read_file(){
	char * file_buffer;
	
	check_validfile(FILEMODE_READ);

	// Set variable 'file_charcount':
	get_file_byte_count();
	file_buffer = (char*) malloc(sizeof(char)*file_charcount);
	
	/* This function (fread) will move the file's pointer into the end of it.
	*  It'll need to be rewinded back.
	*/
	fread(file_buffer, file_charcount+1, 1, fp);  
	rewind(fp); // Bring file's pointer back to the start of the file
	file_buffer[file_charcount] = '\0';
	return file_buffer;
}

void write_file(char * content){
	// Set file's mode to 'append' mode:
	check_validfile(FILEMODE_APPEND);
	fprintf(fp, content);
}

void create_file(char * filepath){
	// Set 'database_file' to the new path:
	database_file = filepath;
	// And create file by opening it:
	open_file(FILEMODE_WRITE);
}

void delete_file(){
	if (isOpen)
		close_file();
	remove(database_file);
}

int get_file_byte_count(){
	int count = 0;

	/*
	* Check if the file is in read mode.
	* If it isn't, reopen in read mode:
	*/
	check_validfile(FILEMODE_READ);

	// Count the characters inside the file:
	while (fgetc(fp) != EOF) 
		count++;
	// Rewind the file's pointer to the start:
	rewind(fp);
	// Set global variable:
	file_charcount = count;
	return count;
}

int get_file_line_count(){
	char ch;
	char nullfind_buff[INPUT_MAXIMUMBUFFER];
	int i = 0;
	int linecount = 0;

	/* 
	* Check if the file is in read mode. 
	* If it isn't, reopen in read mode:
	*/
	check_validfile(FILEMODE_READ);

	// Count the lines inside the file:
	while ((ch = fgetc(fp)) != EOF) {
		if (ch == NEWLINE_ASCII) {
			// Check if there's a string '<NULL'. If there is then ignore this line
			if (strstr(nullfind_buff, NULL_STUDENT_STR) == NULL)
				linecount++;
			i = 0;
		}
		// Prevent 'i' from writing outside the buffer using the if:
		if (i < INPUT_MAXIMUMBUFFER) {
			nullfind_buff[i] = ch;
			i++;
		}
	}

	// Set global variable:
	file_linecount = linecount;
	return linecount;
}

void cleanup_file(){
	int i;

	// Close file first:
	if (isOpen)
		close_file();
	
	// And deallocate everything:
	if (loaded){
		
		// free file contents:
		for (i = 0; i < file_linecount; i++)
			free(database_content[i]);
		free(database_content);
		
		// free database's filepath:
		free(database_file);
		loaded = false;
	}
}