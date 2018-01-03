#ifndef LINKED_LST_H_
#define LINKED_LST_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../Database_def/database_inc.h"

// String to be attributed to the null student:
#define NULL_STUDENT_STR "<NULL>"

// Linked list's structure:
struct node{
	Database_Student * database_node;
	struct node * next_node;
};

// The head of the list from where the list grows:
struct node * list_head;
// To track how many nodes were allocated in the list:
int allocated_nodes;

extern void initialize_linked_list();
extern Database_Student * allocate_database_node(char * id, char * firstName, char * lastName, int day, int month, int year);
extern void push_node(Database_Student * new_student);
extern void insert_node(Database_Student * new_student, int index);
extern void delete_node(int index);
extern Database_Student * get_node(int index);
extern Database_Student ** fetch_all_nodes(); 
extern void cleanup_linked_list();

#endif