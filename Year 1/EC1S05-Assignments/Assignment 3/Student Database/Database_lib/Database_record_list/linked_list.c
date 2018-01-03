#include "linked_list.h"

void initialize_linked_list(){
	// Allocate linked list's head:
	list_head = (struct node*) malloc(sizeof(struct node));
	
	// Fill the head's data with the NULL student:
	list_head->database_node = allocate_database_node(NULL_STUDENT_STR, NULL_STUDENT_STR, NULL_STUDENT_STR, -1, -1, -1);
	
	// And put the next node as a NULL:
	list_head->next_node = NULL;
	
	allocated_nodes = 0;
}

Database_Student * allocate_database_node(char * id, char * firstName, char * lastName, int day, int month, int year){
	// Allocate new database student and set its data using the arguments provided:
	Database_Student * temp = (Database_Student*) malloc(sizeof(Database_Student));
	Date birthDate;
	
	// Initialize id and name:
	strncpy(temp->id, id, ID_MAXSIZE-1);
	strncpy(temp->firstName, firstName, FIRST_NAME_MAXSIZE-1);
	strncpy(temp->lastName, lastName, LAST_NAME_MAXSIZE-1);
	
	// Initialize birth date and set it:
	sprintf(birthDate.day, "%d", day);
	sprintf(birthDate.month, "%d", month);
	sprintf(birthDate.year, "%d", year);
	temp->birthDate = birthDate;

	// Ready to be returned:
	return temp;
}

void push_node(Database_Student * new_student){
	struct node * curr = list_head;
	
	// First find the tail of the linked list:
	while (curr->next_node != NULL) 
		curr = curr->next_node;

	// After it was found, allocate the next node of the 'old' tail:
	curr->next_node = (struct node *) malloc(sizeof(struct node));
	// Set the 'new' tail's data:
	curr->next_node->database_node = new_student;
	// And put the next node of the 'new' tail as a NULL:
	curr->next_node->next_node = NULL;

	allocated_nodes++;
}

void insert_node(Database_Student * new_student, int index){
	struct node * curr = list_head;
	struct node * new_node;
	struct node * tmp;

	// In case the index chosen is bigger than the existing list:
	if (index > allocated_nodes)
		index = allocated_nodes+1;

	/* Find the 'index-th' student by decrementing the index
	 * until it reaches 1
	 */
	while (index > 1){
		curr = curr->next_node;
		index--;
	}

	// First save the next student so it doesn't get lost:
	tmp = curr->next_node;

	// Allocate the new node in order to be inserted:
	new_node = (struct node *) malloc(sizeof(struct node));
	// Set its data:
	new_node->database_node = new_student;

	// And actually insert it in:
	curr->next_node = new_node;
	
	/* At this point, the node that was previously here was lost.
	 * In order to fix this, the next node after the insertion
	 * which was previously stored in this function is now put 
	 * back into the list:
	 */
	new_node->next_node = tmp;

	allocated_nodes++;
}

void delete_node(int index){
	int i = 0;
	struct node * curr = list_head;
	struct node * tmp_node = NULL;

	// If the index is invalid set it to a default value:
	if (index > allocated_nodes)
		index = allocated_nodes;

	// Find the 'index-th' node first by walking through the list:
	for (i = 0; i < index - 1; i++)
		if (curr->next_node == NULL)
			// Something wrong happened at this point. This shouldn't happen in any way.
			return; 
		else 
			curr = curr->next_node;

	/* The node was found. Store the node after the current one
	 * so it can be linked with the previous one:
	 */
	tmp_node = curr->next_node;

	// And link the next with the previous one:
	curr->next_node = tmp_node->next_node;

	// At this point the current node can be deleted:
	free(tmp_node->database_node);
	free(tmp_node);

	allocated_nodes--;
}

Database_Student * get_node(int index){
	int i;
	struct node * ptr = list_head;
	
	/* This 'index_limit' serves the purpose of fetching (or not) the NULL node,
	 * which resides in the index 0 in the linked list
	 */
	int index_limit = 1;
	if (index == NULL) 
		index_limit = 0;

	/* Decrement index until it reaches 'index_limit', which will be a 1 or 0. 
	 * and while doing that, set 'ptr' to its next node. This will allow the program
	 * to find the 'index-th' node on the list.
	 */
	while (index > index_limit){
		ptr = ptr->next_node;
		index--;
	}
	// The node was found. Return it:
	return ptr->database_node;
}

Database_Student ** fetch_all_nodes(){
	Database_Student ** whole_data; 
	int i;

	if (allocated_nodes == 0) {
		// Fetch the NULL student:
		whole_data = (Database_Student**) malloc(sizeof(Database_Student*));
		whole_data[0] = get_node(NULL);
	}
	else {
		// Fetch the whole database:
		whole_data = (Database_Student**) malloc(sizeof(Database_Student*) * allocated_nodes);

		for (i = 2; i <= allocated_nodes + 1; i++)
			whole_data[i - 2] = get_node(i);
	}

	// And return the data:
	return whole_data;
}

void cleanup_linked_list(){
	int i;

	// free all nodes from the linked list:
	for (i = 1; i <= allocated_nodes; i++)
		delete_node(i);

	free(list_head);
}