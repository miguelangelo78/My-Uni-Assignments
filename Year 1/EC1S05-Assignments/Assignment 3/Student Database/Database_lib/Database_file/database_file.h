#ifndef DATABASE_RESOURCES_H_
#define DATABASE_RESOURCES_H_
#include "../Database_def/database_inc.h"
#include "../Misc/stdbool.h"
#include "../Misc/misc.h"
#include "../Database_record_list/linked_list.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Constants for determining the line count of a file:
#define NEWLINE "\n"
#define NEWLINE_ASCII 10

// File constants:
#define FILEMODE_READ				"r"
#define FILEMODE_WRITE				"w"
#define FILEMODE_APPEND				"a"
#define FILEMODE_READWRITE			"r+"
#define FILEMODE_READWRITE_TRUNC	"w+"
#define FILEMODE_READWRITE_APPEND	"a+"
#define FILEMODE_NULL				"NULL"

// Global variables:
char file_currentmode[5];
char * database_file;
char ** database_content;
int file_linecount, file_charcount; 
bool isOpen;
extern bool loaded;
FILE * fp;

// Loading and saving functions:
extern bool load_database(char * database_filepath);
extern void save_database(Database_Student ** database_data, int entry_count);

// General purpose file functions:
extern bool file_exists(char * filename);
extern void check_validfile(char * validmode);
extern void open_file(char * mode);
extern void close_file();
extern char * read_file();
extern void write_file(char * content);
extern void create_file(char * filepath);
extern void delete_file();
extern int get_file_byte_count();
extern int get_file_line_count();
extern void cleanup_file();

#endif