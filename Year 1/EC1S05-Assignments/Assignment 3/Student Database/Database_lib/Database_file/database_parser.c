#include "database_parser.h"

/*
* The format of the records stored is as follows:
* ID: VALUE | First: VALUE | LastName: VALUE | Birth XX/XX/XXXX;
*
* This C module parses this information from a 2D array. This 2D array should already contain the
* file's data, in which each row of the 2D array is a line of the file.
* 
* The linked list is then populated using the data that is parsed inside the function 'parse_database'
*
* Since C doesn't support regex, strtok (which splits a string using a char delimiter) and substrings
* had to be used. 
* The different subtring starting indices are stored within 'substring_start_indices'.
* For example, the first property: ID: VALUE |. The value's first char index starts at '4'.
* Another example, the second property: First: VALUE |. The value's first char index starts at '8'.
* This is what the array 'substring_start_indices' is used for while parsing the data.
*/

// Different start of substrings so the actual data can be extracted:
const int substring_start_indices[4] = { 4, 8, 11, 8 };

Database_Student ** parse_database(char ** database_content, int file_linecount){
	// Read, parse and return database array
	Database_Student ** parsed_records = (Database_Student**) malloc(sizeof(Database_Student*) * file_linecount);
	Date new_date;

	int i;
	int property_index = 0, birthdate_index = 0;
	char * property_matches;
	char * birthdate_match;

	for (i = 0; i < file_linecount; i++){
		parsed_records[i] = (Database_Student*) malloc(sizeof(Database_Student)* PROPERTY_COUNT);
		// walk through all properties:
		property_matches = strtok(database_content[i], PROPERTY_DELIMITER);
		while (property_matches != NULL){

			/* Get the actual data from the property. 
			*  The format is: "PropertyID: VALUE |" (not including the '|' since the string was split using '|'). 
			*  The 'substring()' function extracts the 'VALUE' text without the text 'PropertyID':
			*/
			property_matches = substring(property_matches, substring_start_indices[property_index], strlen(property_matches) - 1);
			
			// Initialize 'parsed_records' with respective value before walking to the next property:
			switch (property_index){
				case PROPERTY_ID:
					strcpy(parsed_records[i]->id, property_matches);
					break;
				case PROPERTY_FIRSTNAME:
					strcpy(parsed_records[i]->firstName, property_matches);
					break;
				case PROPERTY_LASTNAME:
					strcpy(parsed_records[i]->lastName, property_matches);
					break;
				case PROPERTY_BIRTHDATE:
					// If property found is a birth date: Split birthdate into 3
					birthdate_match = strtok(property_matches, BIRTHDATE_DELIMITER);
					while (birthdate_match != NULL){
						// initialize 'new_date' with respective field (birth date) before walking to the next property
						switch (birthdate_index){
							case PROPERTY_BIRTH_DAY:
								strcpy(new_date.day, birthdate_match);
								break;
							case PROPERTY_BIRTH_MONTH:
								strcpy(new_date.month, birthdate_match);
								break;
							case PROPERTY_BIRTH_YEAR:
								strcpy(new_date.year, birthdate_match);
								break;
						}

						// Find next property within the birthdate field:
						birthdate_match = strtok(NULL, BIRTHDATE_DELIMITER);
						birthdate_index++;
					}
					birthdate_index = 0;
					break;
			}

			// Free the property's value and walk to the next property in the current line:
			free(property_matches);
			property_matches = strtok(NULL, PROPERTY_DELIMITER);
			// Indicates which property is being indexed (ID, first name, last name or birth date)
			property_index++;
		}
		
		// And set the date fetched from the match:
		parsed_records[i]->birthDate = new_date;
		property_index = 0;
	}
	return parsed_records;
}