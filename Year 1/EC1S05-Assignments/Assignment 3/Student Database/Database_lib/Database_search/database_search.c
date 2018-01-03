#include "database_search.h"

/* TYPES OF QUERY:
- ID, First, Last and Birth
- Whole match (case sensitive)
- Whole match (case insensitive)
- First letter (insensitive)
- Last letter (insensitive)
- Equal
- Greater (if it's a string then compare its size)
- Lesser (if it's a string then compare its size)

- Search operators:
- Inverting search (all matches that do not match 'keyword')
*/

int * database_query(int search_props, char * keyword){
	Database_Student * current; 
	int * indices_found = NULL;
	int * inverted_indices_found = NULL;
	int indices_count = 0;
	int inverted_indices_found_count = 0;
	int i, i2 = 0;
	char birthday_buffer[WHOLEBIRTH_MAXSIZE];

	// A copy is being created so the original keyword is not altered:
	char * keyword_cpy = (char*) malloc(sizeof(char)*(strlen(keyword) + 1));
	strcpy(keyword_cpy, keyword);

	// Fetch and separate all properties and filters:
	bool inverted_match = search_props & SEARCH_FILTER_INV;
	bool case_sensitive = search_props & SEARCH_FILTER_CASE;
	bool first_letter = search_props & SEARCH_FILTER_FIRSTLETTER;
	bool last_letter = search_props & SEARCH_FILTER_LASTLETTER;
	bool get_greater = search_props & SEARCH_FILTER_GREATER;
	bool get_lesser = search_props & SEARCH_FILTER_LESS;
	char mode = search_props & SEARCH_MODE_MASK;
	char * mode_token;

	// Both cannot be enabled:
	if (first_letter && last_letter)
		first_letter = last_letter = false;
	
	// TODO: Find matches using a keyword and after that increment indices_count

	/* Resize 'indices_found' if a match was found. 
	 * After that store the match in the array
	 */
	for (i = 2; i <= allocated_nodes+1; i++){
		current = get_node(i);

		// TODO: Apply search here:
		
		// Fetch mode token, that is, ID, or FirstName, or LastName, or Birthday (whole) or a component of Birthday
		switch (mode){
			case SEARCH_MODE_ID:
				mode_token = (char*) malloc(sizeof(char)*strlen(current->id) + 1);
				strcpy(mode_token, current->id);
				break;
			case SEARCH_MODE_FIRSTNAME: 
				mode_token = (char*) malloc(sizeof(char)*strlen(current->firstName) + 1);
				strcpy(mode_token, current->firstName);
				break;
			case SEARCH_MODE_LASTNAME: 
				mode_token = (char*) malloc(sizeof(char)*strlen(current->lastName) + 1);
				strcpy(mode_token, current->lastName);
				break;
			case SEARCH_MODE_BIRTH_WHOLE:
				// Construct birthday's string using strcpy and/or strcat:
				
				// Remove each element if the user desired to:
				if (!(search_props & SEARCH_FILTER_DEL_DAY)){
					strcpy(birthday_buffer, current->birthDate.day);
					strcat(birthday_buffer, "/");
				}

				if (!(search_props & SEARCH_FILTER_DEL_MONTH)) {
					// If this happens then the first 'if' of this switch's case wasn't executed:
					if (strlen(birthday_buffer) == 0)
						strcpy(birthday_buffer, current->birthDate.month);
					else
						strcat(birthday_buffer, current->birthDate.month);
					strcat(birthday_buffer, "/");
				}

				if (!(search_props & SEARCH_FILTER_DEL_YEAR)) {
					// If this happens then the first and/or second 'if' of this switch's case wasn't executed:
					if (strlen(birthday_buffer) == 0)
						strcpy(birthday_buffer, current->birthDate.year);
					else
						strcat(birthday_buffer, current->birthDate.year);
				}

				// And store it into the token:
				mode_token = (char*) malloc(sizeof(char)*WHOLEBIRTH_MAXSIZE);
				strcpy(mode_token, birthday_buffer);
				break;
			case SEARCH_MODE_BIRTH_DAY: 
				mode_token = (char*) malloc(sizeof(char)*strlen(current->birthDate.day) + 1);
				strcpy(mode_token, current->birthDate.day);
				break;
			case SEARCH_MODE_BIRTH_MONTH: 
				mode_token = (char*) malloc(sizeof(char)*strlen(current->birthDate.month) + 1);
				strcpy(mode_token, current->birthDate.month);
				break;
			case SEARCH_MODE_BIRTH_YEAR: 
				mode_token = (char*) malloc(sizeof(char)*strlen(current->birthDate.year) + 1);
				strcpy(mode_token, current->birthDate.year);
				break;
			default: 
				mode_token = (char*) malloc(sizeof(char) * strlen(SEARCH_MODE_NULL_MSG)+1);
				strcpy(mode_token, SEARCH_MODE_NULL_MSG);
		}

		// If the user chose only one letter then filter it:
		if (first_letter)
			mode_token[1] = '\0';
		else if (last_letter){
			mode_token[0] = mode_token[strlen(mode_token)-1];
			mode_token[1] = '\0';
		}

		// If it's NOT case sensitive then put them both in capital letters:
		if (!case_sensitive){
			str_toupper(keyword_cpy);
			str_toupper(mode_token);
		}

		// Try to match:
		if (get_greater){
			// Match words that are smaller than the keyword:
			if (strcmp(keyword_cpy, mode_token) >  0){
				// Got match
				indices_count++;
				indices_found = (int*) realloc(indices_found, indices_count*sizeof(int));
				indices_found[indices_count - 1] = i - 1;
			}
		}
		else if (get_lesser){
			// Match words that are bigger than the keyword:
			if (strcmp(keyword_cpy, mode_token) < 0) {
				// Got match
				indices_count++;
				indices_found = (int*) realloc(indices_found, indices_count*sizeof(int));
				indices_found[indices_count - 1] = i - 1;
			}
		}
		else {
			// Match exactly the word:
			if (strcmp(keyword_cpy, mode_token) == 0) {
				// Got match
				indices_count++;
				indices_found = (int*) realloc(indices_found, indices_count*sizeof(int));
				indices_found[indices_count - 1] = i - 1;
			}
		}

		free(mode_token);
	}

	// The search has almost finished.
	// Now invert matches if filter says so:
	if (inverted_match){
		// Match everything that is the opposite of what was originally matched:
		inverted_indices_found_count = allocated_nodes - indices_count; 
		
		inverted_indices_found = (int*) malloc(sizeof(int)*(inverted_indices_found_count + 1));

		if (inverted_indices_found_count > 1){
			
			for (i = 1; i <= allocated_nodes; i++){
				/* See if the current index 'i' exists in the array 'indices_found'.
				 * If it doesn't add it to the array 'inverted_indices_found':
				 */
				if (!arr_contains(indices_found, indices_count, i))
					inverted_indices_found[i2] = i, i2++;
			}

			// Append a NULL because the 'display' function needs it:
			inverted_indices_found[inverted_indices_found_count] = NULL;
		}
		else inverted_indices_found[0] = NULL;
		
		free(keyword_cpy);
		free(indices_found);
		return inverted_indices_found;
	}

	// Append a NULL because the 'display' function needs it:
	indices_count++;
	indices_found = (int*) realloc(indices_found, indices_count*sizeof(int));
	indices_found[indices_count - 1] = NULL; // Done appending the NULL
	free(keyword_cpy);
	return indices_found;
}
