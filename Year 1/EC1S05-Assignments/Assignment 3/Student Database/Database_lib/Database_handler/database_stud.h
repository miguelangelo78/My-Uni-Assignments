#ifndef DATABASE_STUD_H
#define DATABASE_STUD_H

#include "../Database_def/database_inc.h"
#include "../Database_record_list/linked_list.h"
#include "../Database_file/database_file.h"
#include "../Database_file/database_parser.h"
#include "../Database_search/database_search.h"
#include "../Misc/misc.h"

// Simple messages for indicating what task on the database is being done
#define TASKNAME_ADDING		"Adding new student"
#define TASKNAME_INSERTING	"Inserting new student"

// Mode at which the database will be operating:
#define MODE_NEW  'N' // Just created a new database
#define MODE_OPEN 'O' // Just opened a database

// Database functions:
extern void initialize_database(char mode, char * database_path);
extern void save_data();
extern Database_Student * form_create_student();
extern void display_database(int start,int limit_output);
extern void display_list(int * indices,char * search_made);
extern void display(int index);
extern int * search_database(int search_mode, char * keyword);
extern void add_student();
extern void insert_student(int index);
extern void delete_student(int index);
extern int get_database_size();
extern void empty_database();
extern void delete_database();
extern void cleanup_database();

#endif